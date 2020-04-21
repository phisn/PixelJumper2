#pragma once

#include "CachedControllablePlayer.h"

#include "GameCore/ClassicSimulation.h"
#include "GameCore/net/ClassicSimulatorMessage.h"
#include "NetCore/RequestHandler.h"
#include "ResourceCore/pipes/MemoryPipe.h"

#include <deque>

namespace Game
{
	class ArtificialPlayer
	{
		static constexpr float maxSmoothDifference = 1.f;

	public:
		ArtificialPlayer(
			const Resource::PlayerID playerID,
			const Resource::RepresentationID representationID,
			const std::string username)
			:
			playerInfo(
				playerID,
				representationID,
				username)
		{
			representation = PlayerRepresentation::Create(playerInfo);
			representation->setSize({ 1.f , 1.f });
		}

		~ArtificialPlayer()
		{
			delete representation;
		}

		void process(const sf::Time time)
		{
			// think about adding smoothing
			representation->update(time);
		}

		void processLogic()
		{
			if (procesLogicCounter >= NetMovementCaptureTime)
			{
				if (movement.size() > 0)
				{
					source = target;
					target = movement.front();

					movement.pop_front();

					procesLogicCounter = 1;

					sf::Vector2f positiveDifference = target - source;

					if (positiveDifference.x < 0) positiveDifference.x *= -1.f;
					if (positiveDifference.y < 0) positiveDifference.y *= -1.f;

					if (positiveDifference.x > maxSmoothDifference || positiveDifference.y > maxSmoothDifference)
					{
						smoothEnabled = false;
						representation->setPosition(target);
					}
					else
					{
						smoothEnabled = true;
					}

				}
				else return;
			}
			else
			{
				++procesLogicCounter;
			}

			if (smoothEnabled)
			{
				sf::Vector2f current = source;

				current.x += (target.x - source.x) * procesLogicCounter / NetMovementCaptureTime;
				current.y += (target.y - source.y) * procesLogicCounter / NetMovementCaptureTime;

				representation->setPosition(current);
			}
		}

		void draw(sf::RenderTarget* const target)
		{
			representation->draw(target);
		}

		void pushMovement(const NetPlayerMovement* const fullMovement)
		{
			for (sf::Vector2f singleMovement : fullMovement->positions)
				movement.push_back(singleMovement);

			while (movement.size() > NetMovementPushCount)
			{
				movement.pop_front();
			}
		}

		const PlayerInformation& getInformation() const
		{
			return playerInfo;
		}

	private:
		bool smoothEnabled;

		sf::Vector2f source, target;
		std::deque<sf::Vector2f> movement;

		size_t procesLogicCounter = NetMovementCaptureTime;

		PlayerInformation playerInfo;
		PlayerRepresentation* representation;
	};

	struct ClientClassicSimulationHandlerCallback
	{
		virtual void onSimulationClosed() = 0;

		// onSimulationFailure does not notify host
		// host has to be notified manually
		virtual void onSimulationFailure() = 0;
	};

	struct ClientClassicSimulationHandlerArguments
	{
		ClientClassicSimulationHandlerCallback* callback;

		// worldcontainer and players are hold by other modules might
		// adopt over time
		Resource::WorldContainer& worldContainer;
		const std::vector<Resource::PlayerResource*>& players;

		const Resource::PlayerResource& playerResource;
		const SimulationBootInformation& info;
	};

	class ClientClassicSimulationHandler
		:
		public ::Net::RequestHandler,
		public ClassicSimulation
	{
	public:
		ClientClassicSimulationHandler(
			const ClientClassicSimulationHandlerArguments& arguments,
			Device::View* const view,
			const InputID inputID)
			:
			ClassicSimulation(arguments.worldContainer),
			callback(arguments.callback),
			info(arguments.info),
			worldContainer(arguments.worldContainer),
			players(arguments.players),
			player(
				inputID,
				PlayerInformation{
					arguments.playerResource.content.userID,
					info.representationID,
					arguments.playerResource.username
				},
				view)
		{
			ClassicSimulation::player = &player;
		}

		virtual ~ClientClassicSimulationHandler()
		{
		}

		virtual bool initializeSimulation()
		{
			if (runWorld(info.worldID))
			{
				onSimulationStarted();
				return true;
			}
			else
			{
				onSimulationFailed();
				return false;
			}
		}

		void onLogic(const sf::Time time)
		{
			logicCounter += time.asMicroseconds();

			if (gameSpeedAdjusted)
			{
				const float adjustedTimeStep = LogicTimeStep * currentGameSpeed;

				while (logicCounter > adjustedTimeStep)
				{
					logicCounter -= adjustedTimeStep;

					if (!processLogic())
					{
						break;
					}

					if (--remainingGameSpeedAdjustment == 0)
					{
						gameSpeedAdjusted = false;
						break;
					}
				}
			}
			else
			{
				while (logicCounter > LogicTimeStep)
				{
					logicCounter -= LogicTimeStep;
					if (!processLogic())
						break;
				}
			}
		}

		void update() override
		{
			sendBufferedFrames();
		}

		bool onMessage(
			const Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Net::Host::ClassicSimulationMessageID::SimulationFailed:
				Log::Warning(L"simulation at simulator failed, closing");

				// fall
			case Net::Host::ClassicSimulationMessageID::SimulationClosed:
				callback->onSimulationClosed();

				return true;
			case Net::Host::ClassicSimulationMessageID::PushPlayer:
				if (Net::Host::PushPlayerMessage message; loadMessage(messageID, &message, pipe))
				{
					onPushPlayer(message);
				}

				return true;
			case Net::Host::ClassicSimulationMessageID::PopPlayer:
				if (Net::Host::PopPlayerMessage message; loadMessage(messageID, &message, pipe))
				{
					onPopPlayer(message);
				}

				return true;
			case Net::Host::ClassicSimulationMessageID::PushMovement:
			{
				NetPlayerMovement movement;
				Net::Host::PlayerMovementMessage message;
				message.movement = &movement;

				if (loadMessage(messageID, &message, pipe))
				{
					onPlayerMovement(message);
				}

				return true;
			}
			case Net::Host::ClassicSimulationMessageID::TemporarilySpeedAdjustment:
				if (Net::Host::TemporarilySpeedAdjustmentMessage message; loadMessage(messageID, &message, pipe))
				{
					onTemporarilySpeedAdjustment(message);
				}

				return true;
			case Net::Host::ClassicSimulationMessageID::Synchronize:
				if (Net::Host::HostSynchronizeMessage message; loadMessage(messageID, &message, pipe))
				{
					onSynchronize(message);
				}

				return true;
			case Net::Host::ClassicSimulationMessageID::PrepareSync:
				synchronize();

				return true;
			case Net::Host::ClassicSimulationMessageID::PushWorldResource:
				// do currently accept all world resource
				// might filter in the future

				Net::Host::ClassicSimulation::PushWorldResourceMessage message;
				message.world = new Resource::World;

				if (loadMessage(messageID, &message, pipe))
				{
					onPushWorldResource(message);
				}
				else
				{
					delete message.world;
				}

				return true;
			}

			return false;
		}

	protected:
		virtual void onSimulationStarted() = 0;
		virtual void onSimulationFailed() = 0;

		void synchronize()
		{
			// need to send playermovement first to prevent any desynchroization
			// issues caused by buffered movement
			sendBufferedFrames();

			if (access->sendMessage(Net::Client::ClassicSimulationMessageID::RequestSynchronize))
			{
				synchronizing = true;
				synchronizeBeginTick = getTick();
			}
		}

		void sendBufferedFrames()
		{
			if (interrupted)
			{
				Net::Client::ClassicSimulation::PushDelayMessage message;
				message.content.count = interruptedFrameCounter;

				access->sendMessage(
					Net::Client::ClassicSimulationMessageID::PushDelay,
					&message);

				interruptedFrameCounter = 0;
			}
			else
			{
				Net::Client::PushMovementMessage message;
				message.packetFrameStatus = &player.packedFrameStatus;

				access->sendMessage(
					Net::Client::ClassicSimulationMessageID::PushMovement,
					&message);

				player.packedFrameStatus.frames.clear();
			}
		}

		// const information
	private:
		ClientClassicSimulationHandlerCallback* const callback;
		const SimulationBootInformation info;
		Resource::WorldContainer& worldContainer;
		const std::vector<Resource::PlayerResource*>& players;

		// synchronization
	private:
		bool synchronizing;
		std::deque<FrameStatus> synchronizingFrames;
		sf::Uint32 synchronizeBeginTick;

		// game speed adjustment
	private:
		// used to remove tick indifference with server
		bool gameSpeedAdjusted = false;
		float currentGameSpeed;
		sf::Uint64 remainingGameSpeedAdjustment;

		// common simulation specific
	private:
		CachedControllablePlayer player;
		std::vector<ArtificialPlayer*> artificialPlayers;

		sf::Uint64 logicCounter = 0;

		// message handlers
	private:
		void onPushPlayer(const Net::Host::PushPlayerMessage& message)
		{
			Log::Information(L"player joined", message.content.playerID, L"playerID");

			std::string username = "user not found";
			for (const Resource::PlayerResource* player : players)
				if (player->content.userID == message.content.playerID)
				{
					username = player->username;
					break;
				}

			artificialPlayers.push_back(
				new ArtificialPlayer(
					message.content.playerID,
					message.content.representationID,
					std::move(username))
			);
		}

		void onPopPlayer(const Net::Host::PopPlayerMessage& message)
		{
			Log::Information(L"player left", message.content.playerID, L"playerID");

			decltype(artificialPlayers)::iterator iterator = std::find_if(
				artificialPlayers.begin(),
				artificialPlayers.end(),
				[&message](ArtificialPlayer* const artificialPlayer)
				{
					return artificialPlayer->getInformation().playerId == message.content.playerID;
				});

			if (iterator != artificialPlayers.end())
				artificialPlayers.erase(iterator);
		}

		void onPlayerMovement(const Net::Host::PlayerMovementMessage& message)
		{
			Log::Information(L"got player movement", message.movement->content.playerID, L"playerID");
			// if (message.movement->content.worldID != simulation.)

			for (ArtificialPlayer* const artificialPlayer : artificialPlayers)
				if (artificialPlayer->getInformation().playerId == message.movement->content.playerID)
				{
					artificialPlayer->pushMovement(message.movement);
					break;
				}
		}

		void onTemporarilySpeedAdjustment(const Net::Host::TemporarilySpeedAdjustmentMessage& message)
		{
			currentGameSpeed = message.content.speedAdjustment;
			gameSpeedAdjusted = true;
			remainingGameSpeedAdjustment = message.content.speedAdjustmentLength;

			Log::Information(L"speed adjusted",
				currentGameSpeed, L"speed",
				remainingGameSpeedAdjustment, L"length");
		}

		void onSynchronize(const Net::Host::HostSynchronizeMessage& message)
		{
			if (!synchronizing)
			{
				Log::Error(L"synchronize tried without preparation",
					message.state.size(), L"statesize",
					info.worldID, L"worldID",
					info.representationID, L"reprID",
					player.getInformation().playerId, L"userID",
					player.getInformation().name, L"username");

				// simulation failure?

				return;
			}

			synchronizing = false;

			Resource::MemoryReadPipe pipe;
			pipe.adopt(message.state);

			if (!readState(&pipe))
			{
				Log::Error(L"Failed to synchronize simulationhandler",
					info.representationID, L"reprID",
					info.worldID, L"worldID",
					player.getInformation().playerId, L"playerID",
					player.getInformation().name, L"username");

				access->sendMessage(Net::Client::ClassicSimulationMessageID::SimulationFailure);
				callback->onSimulationFailure();

				return;
			}

			if (getTick() < synchronizeBeginTick)
			{
				Log::Warning(L"Got invalid tick for synchronization",
					getTick(), L"new_tick",
					synchronizeBeginTick, L"prepared_tick",
					info.representationID, L"reprID",
					info.worldID, L"worldID",
					player.getInformation().playerId, L"playerID",
					player.getInformation().name, L"username");

				synchronizingFrames.clear();

				return;
			}

			if (getTick() - synchronizeBeginTick != 0)
			{
				Log::Information(L"true, this can be not zero [ccsh]",
					getTick(), L"new_tick",
					synchronizeBeginTick, L"beg_tick",
					synchronizingFrames.size(), L"buffered");
			}

			Log::Information(L"before", player.injectedFrames.size(), L"injected");

			// skipping all frames that the simulator received
			for (int i = getTick() - synchronizeBeginTick; i < synchronizingFrames.size(); ++i)
			{
				player.inject(synchronizingFrames[i]);

				processTick();


				/*
				if (result != ClassicSimulation::WorldFailure::Success)
				{
					access->sendMessage(Net::Client::ClassicSimulationMessageID::SimulationFailure);
					callback->onSimulationFailure();

					Log::Error(L"Failed to process simulation",
						(int)result, L"result");

					break;
				}
				*/
			}

			Log::Information(L"after", player.injectedFrames.size(), L"injected",
				getTick(), L"new_tick",
				synchronizeBeginTick, L"prepared_tick",
				info.representationID, L"reprID",
				info.worldID, L"worldID",
				player.getInformation().playerId, L"playerID",
				player.getInformation().name, L"username");

			synchronizingFrames.clear();
		}

		void onPushWorldResource(const Net::Host::ClassicSimulation::PushWorldResourceMessage& message)
		{
			Resource::WorldContainer::const_iterator iterator = worldContainer.find(message.world->content.id);

			if (iterator != worldContainer.end())
			{
				if (getLoadedWorlds().find(iterator->first) != getLoadedWorlds().end())
				{
					Log::Error(L"got a new resource from pushresource of a already loaded world",
						iterator->first, L"worldID",
						getLoadedWorlds().size(), L"loadedWorlds",
						world->getInformation()->worldId, L"current_worldID");

					delete message.world;
					return;
				}

				delete iterator->second;
			}

			worldContainer[message.world->content.id] = message.world;

			decltype(missingTargetResources)::iterator mtr = std::find(
				missingTargetResources.begin(),
				missingTargetResources.end(), message.world->content.id);

			if (mtr != missingTargetResources.end())
			{
				if (!preloadWorld(message.world->content.id))
				{
					Log::Error(L"failed to preload world waiting and pushed resource",
						message.world->content.id, L"worldID");
				}

				if (interrupted && worldExitEvent.targetWorld == message.world->content.id)
				{
					sendBufferedFrames();
					interrupted = false;

					// does not matter weather we call ours or our childs because
					// the resource is gurranteed to be there as message world
					if (!ClassicSimulation::loadWorldDynamicTransition(worldExitEvent))
					{
						Log::Error(L"world transition after interrupt failed",
							player.getInformation().playerId, L"userID",
							player.getInformation().name, L"name",
							worldExitEvent.targetWorld, L"worldID",
							worldExitEvent.offset.x, L"offset_x",
							worldExitEvent.offset.y, L"offset_y",
							worldExitEvent.targetEntry, L"entryID");

						access->sendMessage(Net::Client::ClassicSimulationMessageID::SimulationFailure);
						callback->onSimulationFailure();
					}
				}
			}
			else
			{
				// will likely never happen
				if (interrupted && worldExitEvent.targetWorld == message.world->content.id)
				{
					Log::Error(L"got wanted world resource while interruption but missing entry in missingtargetresources",
						message.world->content.id, L"worldID",
						missingTargetResources.size(), L"missing_size",
						getLoadedWorlds().size(), L"loaded_worlds");
				}
			}
		}

		// classic simulation
	private:
		bool processLogic()
		{
			if (interrupted)
			{
				++interruptedFrameCounter;

				if (--interruptedTimeout <= 0)
				{
					Log::Error(L"got interrupt timeout",
						worldExitEvent.targetEntry, L"entryID",
						worldExitEvent.targetWorld, L"target_worldID",
						world->getInformation()->worldId, L"active_worldID");

					access->sendMessage(Net::Client::ClassicSimulationMessageID::SimulationFailure);
					callback->onSimulationFailure();

					return false;
				}
			}
			else
			{
				processTick();

				if (synchronizing)
					synchronizingFrames.push_back(player.packedFrameStatus.frames.back());
			}

			for (ArtificialPlayer* const artificialPlayer : artificialPlayers)
			{
				artificialPlayer->processLogic();
				artificialPlayer->process(sf::microseconds(LogicTimeStep));
			}

			return true;
		}

		void onWorldExit() override
		{
			Log::Information(L"reached world exit");
			// wait for simulator confirmation
			// and display some nice animationtew

		}

		void onDynamicTransitionFailure() override
		{
			Log::Error(L"got dynamic transition failure");

			access->sendMessage(
				Net::Client::ClassicSimulationMessageID::SimulationFailure,
				NULL);
			callback->onSimulationFailure();
		}

		// handling missing world resource
		// at dynmaic transition -> interrupt
	private:
		DynamicWorldExitEvent worldExitEvent;
		bool interrupted = false;
		int interruptedTimeout;
		int interruptedFrameCounter;

		bool loadWorldDynamicTransition(const DynamicWorldExitEvent& event) override
		{
			if (worldContainer.find(event.targetWorld) == worldContainer.end())
			{
				sendBufferedFrames();

				worldExitEvent = event;
				interrupted = true;
				interruptedFrameCounter = 0;

				// hardcoded (8s)
				interruptedTimeout = 8'000;

				return true;
			}
			else
			{
				return ClassicSimulation::loadWorldDynamicTransition(event);
			}
		}

		// handling missing target resource
	private:
		std::vector<Resource::WorldID> missingTargetResources;

		void onTargetResourceMissing(Resource::WorldID worldID) override
		{
			sendBufferedFrames();
			missingTargetResources.push_back(worldID);

			Net::Client::ClassicSimulation::RequestWorldResourceMessage message;
			message.content.worldID = worldID;

			access->sendMessage(
				Net::Client::ClassicSimulationMessageID::RequestWorldResource,
				&message);
		}
	};
}
