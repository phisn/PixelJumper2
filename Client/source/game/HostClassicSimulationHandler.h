#pragma once

#include <Client/source/game/ClassicSimulation.h>
#include <Client/source/game/net/ClassicSimulatorMessage.h>
#include <Client/source/game/VirtualPlayer.h>

#include <Client/source/net/RequestHandlerBase.h>

namespace Game::Net
{
	struct SimulationBootInformation
	{
		Resource::WorldId worldID;
		Resource::RepresentationID representationID;
	};

	struct ClassicSimulatorHandlerCallback
	{
		virtual void onSimulationFailed(const ClassicSimulation::WorldFailure reason) = 0;
	};

	class ClassicSimulationHandler
		:
		public ::Net::RequestHandler
	{
	public:
		ClassicSimulationHandler(
			ClassicSimulatorHandlerCallback* const callback,
			const WorldResourceContainer& container,
			const Game::PlayerInformation userInfo,
			const SimulationBootInformation bootInfo)
			:
			callback(callback),
			simulation(container),
			player(userInfo),
			bootInfo(bootInfo)
		{
			simulation.setPlayer(&player);
		}

		// needs custom initialize function because a simulation
		// can fail to boot up
		bool initialize()
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

		void processLogic()
		{
			if (player.hasUpdate())
			{
				const ClassicSimulation::WorldFailure result = simulation.processLogic();

				if (result != ClassicSimulation::WorldFailure::Success)
					processLogicFailed(result);
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
				break;
			case Client::ClassicSimulatorMessageID::PushMovement:
				if (Client::PushMovementMessage message; loadMessage(messageID, &message, pipe))
				{
					onPushMovement(message);
				}

				break;
			case Client::ClassicSimulatorMessageID::RequestSync:
				break;
			}
		}

	private:
		ClassicSimulatorHandlerCallback* const callback;
		const SimulationBootInformation bootInfo;

		VirtualPlayer player;
		ClassicSimulation simulation;

		sf::Uint64 missingFrames = 0;

		virtual void onPushMovement(const Client::PushMovementMessage& message)
		{
			// needs more checks later
			for (const FrameStatus& frameStatus : message.packetFrameStatus.frames)
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
