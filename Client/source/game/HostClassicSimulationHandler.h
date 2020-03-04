#pragma once

#include <Client/source/game/ClassicSimulation.h>
#include <Client/source/game/net/ClassicSimulatorMessage.h>
#include <Client/source/game/net/NetPlayerMovement.h>
#include <Client/source/game/SimulatorContext.h>
#include <Client/source/game/VirtualPlayer.h>

#include <Client/source/net/RequestHandlerBase.h>

namespace Game::Net
{
	struct ClassicSimulatorHandlerCallback
	{
		virtual void onSimulationFailed(const ClassicSimulation::WorldFailure reason) = 0;
	};

	// currently transitive reached worlds are not sent to the 
	// player so he wont be able to load other worlds than the
	// initial ones
	// player worlds will potentially send multiple times. it has
	// to be logged if the world was already sent
	class ClassicSimulationHandler
		:
		public ::Net::RequestHandler,
		public SimulatorContextLocationCallback
	{
	public:
		ClassicSimulationHandler(
			ClassicSimulatorHandlerCallback* const callback,
			SimulatorContext& context,
			const WorldResourceContainer& container,
			const Game::PlayerInformation userInfo,
			const SimulationBootInformation bootInfo)
			:
			callback(callback),
			context(context),
			simulation(container),
			player(userInfo),
			bootInfo(bootInfo)
		{
			simulation.setPlayer(&player);

			locationPlayer = new LocationPlayer();
			locationPlayer->callback = this;
			locationPlayer->playerID = userInfo.playerId;
			locationPlayer->representationID = userInfo.representationID;

			movementBuffer.content.playerID = userInfo.playerId;

			simulation.onWorldLoad.addListener(
				[this](Game::World* const world) -> void
				{
					if (location != NULL)
					{
						location->pushMovement(&movementBuffer);
						location->removePlayer(locationPlayer->playerID);
					}

					location = this->context.putPlayer(
						locationPlayer,
						world->getInformation()->worldId);

					movementBuffer.content.worldID = world->getInformation()->worldId;
				});
		}

		~ClassicSimulationHandler()
		{
			location->removePlayer(locationPlayer->playerID);
			delete locationPlayer;
		}

		// needs custom initialize function because a simulation
		// can fail to boot up
		bool initializeSimulation()
		{
			const Game::ClassicSimulation::WorldFailure result = simulation.runWorld(this->bootInfo.worldID);

			if (result != Game::ClassicSimulation::WorldFailure::Success)
			{
				Host::SimulationFailureMessage message;
				message.reason = result;

				access->accessSendMessage(
					Host::ClassicSimulatorMessageID::SimulationFailed,
					&message);

				Log::Error(L"Failed to initiate simulation",
					(int) result, L"result",
					bootInfo.worldID, L"worldID",
					player.getInformation().playerId, L"playerID",
					bootInfo.representationID, L"reprID");

				return false;
			}

			return true;
		}

		void initialize(::Net::ConnectionAccess* const access) override
		{
			RequestHandler::initialize(access);

			for (LocationPlayer* const player : location->players)
				if (player->playerID != locationPlayer->playerID)
				{
					Host::PushPlayerMessage message;
					message.playerID = player->playerID;
					message.representationID = player->representationID;

					access->accessSendMessage(
						Host::ClassicSimulatorMessageID::PushPlayer,
						&message);
				}
		}

		void processLogic()
		{
			if (player.hasUpdate())
			{
				const ClassicSimulation::WorldFailure result = simulation.processLogic();

				if (result != ClassicSimulation::WorldFailure::Success)
					processLogicFailed(result);

				if (++processLogicCounter > NetMovementCaptureTime)
				{
					movementBuffer.positions.push_back(
						player.getProperties().position
					);

					if (movementBuffer.positions.size() > NetMovementPushCount)
						location->pushMovement(&movementBuffer);

					processLogicCounter = 0;
				}
			}
			else
			{
				++missingFrames;
			}
		}

		void update() override
		{
		}

		bool onMessage(
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Client::ClassicSimulatorMessageID::AcceptSync:

				return true;
			case Client::ClassicSimulatorMessageID::PushMovement:
			{
				PackedFrameStatus packedFrameStatus;

				Client::PushMovementMessage message;
				message.packetFrameStatus = &packedFrameStatus;

				if (loadMessage(messageID, &message, pipe))
				{
					onPushMovement(message);
				}

				return true;
			}
			case Client::ClassicSimulatorMessageID::RequestSync:

				return true;
			}

			return false;
		}

	private:
		ClassicSimulatorHandlerCallback* const callback;
		SimulatorContext& context;
		const SimulationBootInformation bootInfo;

		VirtualPlayer player;
		ClassicSimulation simulation;

		LocationPlayer* locationPlayer;
		Location* location = NULL;
		NetPlayerMovement movementBuffer;

		size_t processLogicCounter = 0;
		sf::Uint64 missingFrames = 0;

		virtual void onPushMovement(const Client::PushMovementMessage& message)
		{
			// needs more checks later
			for (const FrameStatus& frameStatus : message.packetFrameStatus->frames)
				player.pushUpdate(frameStatus);

			// TODO: think about making this async
			while (missingFrames > 0 && player.hasUpdate())
			{
				const ClassicSimulation::WorldFailure result = simulation.processLogic();

				if (result != ClassicSimulation::WorldFailure::Success)
				{
					processLogicFailed(result);

					// skiping other entries because simulation
					// is going to be closed anyway
					break;
				}

				--missingFrames;
			}
		}

		void onNetPlayerMovementPush(NetPlayerMovement* const movement) override
		{
			Host::PlayerMovementMessage message;
			message.movement = movement;

			access->accessSendMessage(
				Host::ClassicSimulatorMessageID::PushMovement,
				&message);
		}

		void onNetPlayerAdded(LocationPlayer* const locationPlayer) override
		{
			Host::PushPlayerMessage message;
			message.playerID = locationPlayer->playerID;
			message.representationID = locationPlayer->representationID;

			access->accessSendMessage(
				Host::ClassicSimulatorMessageID::PushPlayer,
				&message);
		}

		void onNetPlayerRemoved(const Resource::PlayerID playerID) override
		{
			Host::PopPlayerMessage message;
			message.playerID = playerID;

			access->accessSendMessage(
				Host::ClassicSimulatorMessageID::PushPlayer,
				&message);
		}

		void processLogicFailed(const ClassicSimulation::WorldFailure result)
		{
			Host::SimulationFailureMessage message;
			message.reason = result;

			access->accessSendMessage(
				Host::ClassicSimulatorMessageID::SimulationFailed,
				&message);

			Log::Error(L"Failed to process simulation",
				(int) result, L"result");
		}
	};
}
