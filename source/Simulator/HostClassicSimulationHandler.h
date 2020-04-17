#pragma once

#include "GameCore/ClassicSimulation.h"
#include "GameCore/net/ClassicSimulatorMessage.h"
#include "GameCore/net/NetPlayerMovement.h"
#include "GameCore/net/SimulatorContext.h"
#include "GameCore/net/VirtualPlayer.h"

#include "NetCore/RequestHandler.h"
#include "ResourceCore/pipes/MemoryPipe.h"

namespace Game
{
	struct ClassicSimulatorHandlerCallback
	{
		virtual void onSimulationFailed(const ClassicSimulation::WorldFailure reason) = 0;
		virtual void onSimulationClosed() = 0;
		virtual void onSimulationFinished(Resource::WorldID worldID) = 0;
	};

	// currently transitive reached worlds are not sent to the 
	// player so he wont be able to load other worlds than the
	// initial ones
	// player worlds will potentially send multiple times. it has
	// to be logged if the world was already sent
	class ClassicSimulationHandler
		:
		public ::Net::RequestHandler,
		public SimulatorContextLocationCallback,
		public ClassicSimulation
	{
		// time in server ticks
		static constexpr sf::Uint64 SpeedAdjustmentTimeOffset = 500;
		static constexpr sf::Uint64 MaxFrameDifference = 1000;
		// has to be higher than networkingsockets timeout
		static constexpr sf::Uint64 MinimalDifferenceVairant = 30'000;
		static constexpr sf::Uint64 ToleratedFrameDifference = 50;

		static constexpr float ToleratedSpeedDifference = 0.2f;

	public:
		ClassicSimulationHandler(
			ClassicSimulatorHandlerCallback* const callback,
			SimulatorContext& context,
			const Resource::WorldContainer& container,
			const Game::PlayerInformation userInfo,
			const SimulationBootInformation bootInfo)
			:
			ClassicSimulation(container),
			callback(callback),
			context(context),
			player(userInfo),
			bootInfo(bootInfo)
		{
			setPlayer(&player);

			locationPlayer = new LocationPlayer();
			locationPlayer->callback = this;
			locationPlayer->playerID = userInfo.playerId;
			locationPlayer->representationID = userInfo.representationID;

			movementBuffer.content.playerID = userInfo.playerId;
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
			const Game::ClassicSimulation::WorldFailure result = runWorld(this->bootInfo.worldID);

			if (result != Game::ClassicSimulation::WorldFailure::Success)
			{
				sendSimulationFailed(Net::Host::ClassicSimulation::SimulationFailureReason::WorldFailure);

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
					::Net::Host::PushPlayerMessage message;
					message.content.playerID = player->playerID;
					message.content.representationID = player->representationID;

					access->sendMessage(
						::Net::Host::ClassicSimulationMessageID::PushPlayer,
						&message);
				}
		}

		void processLogic()
		{
			if (player.hasUpdate())
			{
				processSimulationLogic();
			}
			else
			{
				++missingFrames;
			}
		}

		void update() override
		{

			/*
				player client can potentially be faster than the server
				or have too many frames sent at the beginning
				to compensate these frames the client speed is adjusted to
				have zero remaining frames in SpeedAdjustmentTime
				this process is triggered when the amount of remaining frames
				is above ToleratedFrameDifference and removes the client when
				the amount is above MaxFrameDifference
				SpeedAdjustmentTimeOffset is used to compensate lag between
				clients
				this process is repeated if the remaining frames are still
				above ToleratedFrameDifference after the process and the player
				connection is not aborted because the player client could
				potentially always run at some minor higher speed like 0.05%
			*/
			if (adjustedGameSpeed)
			{
				if ((remainingAdjustedGameSpeed -= 100) <= 0)
					adjustedGameSpeed = false;
			}
			else
			{
				const size_t playerFrameDifference = missingFrames != 0
					? missingFrames
					: player.getFrameCount();

				if (playerFrameDifference > ToleratedFrameDifference)
				{
					if (playerFrameDifference > MaxFrameDifference)
					{
						if (missingFrames != 0)
						{
							differenceVariant -= missingFrames;

							for (int i = 0; i < missingFrames; ++i)
							{
								player.pushEmptyUpdate();
								
								if (!processSimulationLogic())
									break;

								--missingFrames;
							}
						}
						else
						{

							differenceVariant += player.getFrameCount();
							player.clearFrames();
						}

						if (differenceVariant > MaxFrameDifference ||
							differenceVariant < MinimalDifferenceVairant)
						{
							access->onThreatIdentified(
								-1,
								L"too high differencevariant. speed hack detected",
								::Net::ThreatLevel::Malicious);

							sendSimulationFailed(Net::Host::ClassicSimulation::SimulationFailureReason::InvalidSpeed);
							callback->onSimulationClosed();
						}
						else
						{
							access->onThreatIdentified(
								-1,
								L"high client speed indifference",
								::Net::ThreatLevel::Uncommon);

							access->sendMessage(Net::Host::ClassicSimulationMessageID::PrepareSync);
						}
					}
					else
					{
						adjustedGameSpeed = true;
						::Net::Host::TemporarilySpeedAdjustmentMessage message;

						message.content.speedAdjustment = 1.f + (missingFrames != 0 ? -1.f : +1.f)
							* ToleratedSpeedDifference;
						message.content.speedAdjustmentLength = playerFrameDifference / ToleratedSpeedDifference;
						// because of many other factors that will change framedifferences
						// often the is only the half length. this way a periodic speed adjustment
						// because of following too high speed adjustments is prevented
						message.content.speedAdjustmentLength *= 0.5f;

						access->sendMessage(
							::Net::Host::ClassicSimulationMessageID::TemporarilySpeedAdjustment,
							&message);

						remainingAdjustedGameSpeed = message.content.speedAdjustmentLength + SpeedAdjustmentTimeOffset;

						Log::Information(L"speed adjustment",
							message.content.speedAdjustment, L"speed",
							message.content.speedAdjustmentLength, L"length");
					}
				}
			}

			static int i = 0;
			static int min1 = 0, min2 = 0, max1 = 0, max2 = 0, avg1 = 0, avg2 = 0;

			if (min1 > missingFrames)
			{
				min1 = missingFrames;
			}

			if (max1 < missingFrames)
			{
				max1 = missingFrames;
			}

			if (min2 > player.getFrameCount())
			{
				min2 = player.getFrameCount();
			}

			if (max2 < player.getFrameCount())
			{
				max2 = player.getFrameCount();
			}

			avg1 += missingFrames;
			avg2 += player.getFrameCount();

			if (++i >= 5)
			{
				i = 0;
				Log::Information(L"process100",
					missingFrames, L"missingframes",
					min1, L"min",
					max1, L"max",
					avg1 / 5.f, L"avg",
					player.getFrameCount(), L"awaiting",
					min2, L"min",
					max2, L"max",
					avg2 / 5.f, L"avg");

				avg1 = 0;
				avg2 = 0;

				std::swap(min1, max1);
				std::swap(min2, max2);
			}
		}

		bool onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case ::Net::Client::ClassicSimulationMessageID::PushMovement:
			{
				PackedFrameStatus packedFrameStatus;

				::Net::Client::PushMovementMessage message;
				message.packetFrameStatus = &packedFrameStatus;

				if (loadMessage(messageID, &message, pipe))
				{
					onPushMovement(message);
				}

				return true;
			}
			case ::Net::Client::ClassicSimulationMessageID::RequestSynchronize:
				onRequestSynchronize();

				return true;
			case Net::Client::ClassicSimulationMessageID::EnsureWorldResource:
				if (Net::Client::ClassicSimulation::EnsureWorldResourceMessage message; loadMessage(messageID, &message, pipe))
				{
					onEnsureWorldResource(message);
				}

				return true;
			case Net::Client::ClassicSimulationMessageID::RequestWorldResource:
				if (Net::Client::ClassicSimulation::RequestWorldResourceMessage message; loadMessage(messageID, &message, pipe))
				{
					onRequestWorldResource(message);
				}

				return true;
			}

			return false;
		}

	private:
		ClassicSimulatorHandlerCallback* const callback;
		SimulatorContext& context;
		const SimulationBootInformation bootInfo;

		VirtualPlayer player;

		LocationPlayer* locationPlayer;
		Location* location = NULL;
		NetPlayerMovement movementBuffer;

		size_t processLogicCounter = 0;
		sf::Uint64 missingFrames = 0;

		// used to determine if overflow of frames
		// is caused by bad connection or speedhack
		sf::Int64 differenceVariant = 0;

		bool adjustedGameSpeed = false;
		sf::Int64 remainingAdjustedGameSpeed;

		virtual void onPushMovement(const ::Net::Client::PushMovementMessage& message)
		{
			// needs more checks later
			for (const FrameStatus& frameStatus : message.packetFrameStatus->frames)
			{
				player.pushUpdate(frameStatus);
			}

			// TODO: think about making this async
			while (missingFrames > 0 && player.hasUpdate())
			{
				if (!processSimulationLogic())
				{
					// skiping other entries because simulation
					// is going to be closed anyway
					break;
				}

				--missingFrames;
			}
		}

		bool processSimulationLogic()
		{
			if (getStatus() != ClassicSimulation::Status::Running)
			{
				sendSimulationFailed(Net::Host::ClassicSimulation::SimulationFailureReason::InvalidStatus);
				callback->onSimulationClosed();

				return false;
			}

			const ClassicSimulation::WorldFailure result = processTick();

			if (result != ClassicSimulation::WorldFailure::Success)
			{
				processLogicFailed(result);
				return false;
			}

			if (++processLogicCounter > NetMovementCaptureTime)
			{
				movementBuffer.positions.push_back(
					player.getProperties().position
				);

				if (movementBuffer.positions.size() >= NetMovementPushCount)
				{
					location->pushMovement(&movementBuffer);
				}

				processLogicCounter = 1;
			}

			return true;
		}

		/*
		void onEnsureWorldResource(const Net::Client::ClassicSimulation::EnsureWorldResourceMessage& request)
		{
			if (!checkWorldAvailability(request.content.worldID))
			{
				Log::Error(L"user tried to request unavailable worldID [ensure]",
					request.content.worldID, L"worldID",
					player.getInformation().playerId, L"userID");

				return;
			}

			Resource::WorldContainer::const_iterator world = worldResources.find(request.content.worldID);

			if (world == worldResources.cend())
			{
				// is usaslly impossible
				Log::Error(L"failed to retrive world resource in sendpushworldresource",
					request.content.worldID, L"worldID",
					player.getInformation().playerId, L"userID");

				return;
			}

			if (world->second->content.checksum != request.content.checksum)
			{
				Net::Host::ClassicSimulation::PushWorldResourceMessage message;
				message.world = world->second;

				access->sendMessage(
					Net::Host::ClassicSimulationMessageID::PushWorldResource,
					&message);
			}
		}

		void onRequestWorldResource(const Net::Client::ClassicSimulation::RequestWorldResourceMessage& message)
		{
			if (!checkWorldAvailability(message.content.worldID))
			{
				Log::Error(L"user tried to request unavailable worldID",
					message.content.worldID, L"worldID",
					player.getInformation().playerId, L"userID");

				return;
			}

			sendPushWorldResource(message.content.worldID);
		}
		*/

		void sendPushWorldResource(Resource::WorldID worldID)
		{
			Resource::WorldContainer::const_iterator world = worldResources.find(worldID);

			if (world == worldResources.cend())
			{
				// is usaslly impossible
				Log::Error(L"failed to retrive world resource in sendpushworldresource",
					worldID, L"worldID",
					player.getInformation().playerId, L"userID");

				return;
			}

			Net::Host::ClassicSimulation::PushWorldResourceMessage message;
			message.world = world->second;

			access->sendMessage(
				Net::Host::ClassicSimulationMessageID::PushWorldResource,
				&message);
		}

		bool checkWorldAvailability(Resource::WorldID worldID)
		{
			return !(worldID != world->getInformation()->worldId &&
				std::find(
					world->getTargets().begin(),
					world->getTargets().end(),
					worldID) == world->getTargets().end());
		}

		void onNetPlayerMovementPush(NetPlayerMovement* const movement) override
		{
			::Net::Host::PlayerMovementMessage message;
			message.movement = movement;

			access->sendMessage(
				::Net::Host::ClassicSimulationMessageID::PushMovement,
				&message,
				k_nSteamNetworkingSend_Unreliable);
		}

		void onNetPlayerAdded(LocationPlayer* const locationPlayer) override
		{
			::Net::Host::PushPlayerMessage message;
			message.content.playerID = locationPlayer->playerID;
			message.content.representationID = locationPlayer->representationID;

			access->sendMessage(
				::Net::Host::ClassicSimulationMessageID::PushPlayer,
				&message);
		}

		void onNetPlayerRemoved(const Resource::PlayerID playerID) override
		{
			::Net::Host::PopPlayerMessage message;
			message.content.playerID = playerID;

			access->sendMessage(
				::Net::Host::ClassicSimulationMessageID::PushPlayer,
				&message);
		}

		void onRequestSynchronize()
		{
			Resource::MemoryWritePipe pipe;
			
			if (!writeState(&pipe))
			{
				Log::Error(L"Failed to writestates in synchronize",
					bootInfo.representationID, L"reprID",
					bootInfo.worldID, L"worldID",
					player.getInformation().name, L"name",
					player.getInformation().playerId, L"playerID");

				access->sendMessage(Net::CommonMessageID::InternalError);
				return;
			}

			Net::Host::HostSynchronizeMessage message;
			pipe.swap(message.state);

			access->sendMessage(
				Net::Host::ClassicSimulationMessageID::Synchronize,
				&message);
		}

		void processLogicFailed(const ClassicSimulation::WorldFailure result)
		{
			sendSimulationFailed(Net::Host::ClassicSimulation::SimulationFailureReason::WorldFailure);

			Log::Error(L"Failed to process simulation",
				(int) result, L"result");
		}

		bool sendSimulationFailed(Net::Host::ClassicSimulation::SimulationFailureReason reason)
		{
			Net::Host::ClassicSimulation::SimulationFailureMessage message;
			message.content.reason = reason;

			return access->sendMessage(
				Net::Host::ClassicSimulationMessageID::SimulationFailed,
				&message);
		}

		bool onInitializeWorld(World* const world) override
		{
			if (ClassicSimulationHandler::onInitializeWorld(world))
			{
				Resource::WorldContainer::const_iterator iterator = worldResources.find(world->getInformation()->worldId);

				if (iterator != worldResources.end())
				{
					// usally impossible to reach
					Log::Error(L"got invalid resourceid in initializeworld",
						world->getInformation()->worldId, L"worldID");

					return;
				}

				Net::Host::ClassicSimulation::PushWorldResourceMessage message;
				message.world = iterator->second;

				access->sendMessage(
					Net::Host::ClassicSimulationMessageID::PushWorldResource,
					&message);
			}
		}

		void onWorldLoad(World* loadingWorld) override
		{
			if (location != NULL)
			{
				location->pushMovement(&movementBuffer);
				location->removePlayer(locationPlayer->playerID);
			}

			location = this->context.putPlayer(
				locationPlayer,
				loadingWorld->getInformation()->worldId);

			movementBuffer.content.worldID = loadingWorld->getInformation()->worldId;
		}

		void onWorldExit() override
		{
			access->sendMessage(
				Net::Host::ClassicSimulationMessageID::SimulationClosed,
				NULL);
			callback->onSimulationFinished(world->getInformation()->worldId);
		}
	};
}
