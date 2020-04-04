#pragma once

#include "GameCore/ClassicSimulation.h"
#include "GameCore/net/ClassicSimulatorMessage.h"
#include "GameCore/net/NetPlayerMovement.h"
#include "GameCore/net/SimulatorContext.h"
#include "GameCore/net/VirtualPlayer.h"

#include "NetCore/RequestHandler.h"

namespace Game
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
		static constexpr sf::Uint64 SpeedAdjustmentTimeOffset = 1000; // 1s
		static constexpr sf::Uint64 SpeedAdjustmentTime = 5000; // 5s
		static constexpr sf::Uint64 MaxFrameDifference = 1000; // 1s
		static constexpr sf::Uint64 ToleratedFrameDifference = 100; // 100ms

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
				::Net::Host::SimulationFailureMessage message;
				message.reason = result;

				access->sendMessage(
					::Net::Host::ClassicSimulatorMessageID::SimulationFailed,
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
					::Net::Host::PushPlayerMessage message;
					message.playerID = player->playerID;
					message.representationID = player->representationID;

					access->sendMessage(
						::Net::Host::ClassicSimulatorMessageID::PushPlayer,
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
				if (remainingAdjustedGameSpeed -= 100 <= 0)
					adjustedGameSpeed = false;
			}
			else
			{
				const size_t playerFrameDifference = missingFrames != 0
					? missingFrames
					: player.getFrameCount();

				if (playerFrameDifference > ToleratedFrameDifference)
				{
					Log::Information(L"speed indifference",
						playerFrameDifference, L"difference",
						ToleratedFrameDifference, L"max");

					if (playerFrameDifference > MaxFrameDifference)
					{
						if (missingFrames != 0)
						{
							access->onThreatIdentified(
								-1,
								L"invalid client speed",
								::Net::ThreatLevel::Uncommon);

							for (int i = 0; i < missingFrames; ++i)
							{
								player.pushEmptyUpdate();
								
								if (!processSimulationLogic())
									break;
							}
						}
						else
						{
							access->onThreatIdentified(
								-1,
								L"invalid client speed",
								::Net::ThreatLevel::Malicious);

							player.clearFrames();
						}

						access->sendMessage(Net::Host::ClassicSimulatorMessageID::PrepareSync);
					}
					else
					{
						adjustedGameSpeed = true;
						remainingAdjustedGameSpeed = SpeedAdjustmentTime + SpeedAdjustmentTimeOffset;

						::Net::Host::TemporarilySpeedAdjustmentMessage message;

						message.speedAdjustment = (float)SpeedAdjustmentTime /
							(
							(float)SpeedAdjustmentTime + (missingFrames != 0
								? (float)playerFrameDifference
								: -(float)playerFrameDifference)
								);
						// because of many other factors that will change framedifferences
						// often the is only the half length. this way a periodic speed adjustment
						// because of following too high speed adjustments is prevented
						message.speedAdjustmentLength = SpeedAdjustmentTime / 2.f;

						access->sendMessage(
							::Net::Host::ClassicSimulatorMessageID::TemporarilySpeedAdjustment,
							&message);

						Log::Information(L"speed adjustment",
							message.speedAdjustment, L"speed",
							message.speedAdjustmentLength, L"length");
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

			if (++i > 5)
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
			case ::Net::Client::ClassicSimulatorMessageID::AcceptSync:

				return true;
			case ::Net::Client::ClassicSimulatorMessageID::PushMovement:
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
			case ::Net::Client::ClassicSimulatorMessageID::RequestSync:


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
			const ClassicSimulation::WorldFailure result = simulation.processLogic();

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

		void onNetPlayerMovementPush(NetPlayerMovement* const movement) override
		{
			::Net::Host::PlayerMovementMessage message;
			message.movement = movement;

			access->sendMessage(
				::Net::Host::ClassicSimulatorMessageID::PushMovement,
				&message,
				k_nSteamNetworkingSend_Unreliable);
		}

		void onNetPlayerAdded(LocationPlayer* const locationPlayer) override
		{
			::Net::Host::PushPlayerMessage message;
			message.playerID = locationPlayer->playerID;
			message.representationID = locationPlayer->representationID;

			access->sendMessage(
				::Net::Host::ClassicSimulatorMessageID::PushPlayer,
				&message);
		}

		void onNetPlayerRemoved(const Resource::PlayerID playerID) override
		{
			::Net::Host::PopPlayerMessage message;
			message.playerID = playerID;

			access->sendMessage(
				::Net::Host::ClassicSimulatorMessageID::PushPlayer,
				&message);
		}

		void onRequestSync()
		{
			Resource::MemoryWritePipe pipe;
			
			if (!simulation.writeState(&pipe) ||
				!player.writeState(&pipe))
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
			pipe.assign(message.content);

			access->sendMessage(
				Net::Host::ClassicSimulatorMessageID::Synchronize,
				&message);
		}

		void processLogicFailed(const ClassicSimulation::WorldFailure result)
		{
			::Net::Host::SimulationFailureMessage message;
			message.reason = result;

			access->sendMessage(
				::Net::Host::ClassicSimulatorMessageID::SimulationFailed,
				&message);

			Log::Error(L"Failed to process simulation",
				(int) result, L"result");
		}
	};
}
