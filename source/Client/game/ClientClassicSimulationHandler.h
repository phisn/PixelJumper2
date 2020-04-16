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
		WorldResourceContainer& worldContainer;
		const std::vector<Resource::PlayerResource*>& players;

		std::string& username;
		Operator::UserID userID;

		const SimulationBootInformation& info;
	};

	class ClientClassicSimulationHandler
		:
		public ::Net::RequestHandler
	{
	public:
		ClientClassicSimulationHandler(
			const ClientClassicSimulationHandlerArguments& arguments,
			Device::View* const view,
			const InputID inputID)
			:
			callback(arguments.callback),
			info(arguments.info),
			worldContainer(arguments.worldContainer),
			players(arguments.players),
			player(
				inputID,
				PlayerInformation{
					arguments.userID,
					info.representationID,
					arguments.username
				},
				view),
			simulation(arguments.worldContainer)
		{
			simulation.setPlayer(&player);
		}

		virtual ~ClientClassicSimulationHandler()
		{
		}

		virtual bool initializeSimulation()
		{
			const ClassicSimulation::WorldFailure result = simulation.runWorld(info.worldID);

			if (result == ClassicSimulation::WorldFailure::Success)
			{
				onSimulationStarted();
				return true;
			}
			else
			{
				onSimulationFailed(result);
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

		virtual void onDraw(sf::RenderTarget* const target)
		{
			simulation.draw(target);

			for (ArtificialPlayer* const artificialPlayer : artificialPlayers)
				artificialPlayer->draw(target);

			player.onDraw(target);
		}

		void update() override
		{
			sendPlayerMovement();
		}

		bool onMessage(
			const Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Net::Host::ClassicSimulatorMessageID::SimulationClosed:
			case Net::Host::ClassicSimulatorMessageID::SimulationFailed:
				Log::Information(L"closed");

				callback->onSimulationClosed();

				return true;
			case Net::Host::ClassicSimulatorMessageID::PushPlayer:
				if (Net::Host::PushPlayerMessage message; loadMessage(messageID, &message, pipe))
				{
					onPushPlayer(message);
				}

				return true;
			case Net::Host::ClassicSimulatorMessageID::PopPlayer:
				if (Net::Host::PopPlayerMessage message; loadMessage(messageID, &message, pipe))
				{
					onPopPlayer(message);
				}

				return true;
			case Net::Host::ClassicSimulatorMessageID::PushMovement:
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
			case Net::Host::ClassicSimulatorMessageID::TemporarilySpeedAdjustment:
				if (Net::Host::TemporarilySpeedAdjustmentMessage message; loadMessage(messageID, &message, pipe))
				{
					onTemporarilySpeedAdjustment(message);
				}

				return true;
			case Net::Host::ClassicSimulatorMessageID::Synchronize:
				if (Net::Host::HostSynchronizeMessage message; loadMessage(messageID, &message, pipe))
				{
					onSynchronize(message);
				}

				return true;
			case Net::Host::ClassicSimulatorMessageID::PrepareSync:
				synchronize();

				return true;
			}

			return false;
		}

	protected:
		virtual void onSimulationStarted() = 0;
		virtual void onSimulationFailed(const ClassicSimulation::WorldFailure reason) = 0;

		void synchronize()
		{
			// need to send playermovement first to prevent any desynchroization
			// issues caused by buffered movement
			sendPlayerMovement();

			if (access->sendMessage(Net::Client::ClassicSimulatorMessageID::RequestSynchronize))
			{
				synchronizing = true;
				synchronizeBeginTick = simulation.getTick();
			}
		}

		void sendPlayerMovement()
		{
			Net::Client::PushMovementMessage message;
			message.packetFrameStatus = &player.packedFrameStatus;

			access->sendMessage(
				Net::Client::ClassicSimulatorMessageID::PushMovement,
				&message);

			player.packedFrameStatus.frames.clear();
		}

	private:
		bool synchronizing;
		std::deque<FrameStatus> synchronizingFrames;
		sf::Uint32 synchronizeBeginTick;

	private:
		ClientClassicSimulationHandlerCallback* const callback;
		const SimulationBootInformation info;
		const WorldResourceContainer& worldContainer;

		VisualClassicSimulation simulation;
		CachedControllablePlayer player;

		std::vector<ArtificialPlayer*> artificialPlayers;
		const std::vector<Resource::PlayerResource*>& players;

		sf::Uint64 logicCounter = 0;

		// used to remove tick indifference with server
		bool gameSpeedAdjusted = false;
		float currentGameSpeed;
		sf::Uint64 remainingGameSpeedAdjustment;

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
				new ArtificialPlayer(message.playerID, message.representationID, std::move(username))
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

			if (!simulation.readState(&pipe))
			{
				Log::Error(L"Failed to synchronize simulationhandler",
					info.representationID, L"reprID",
					info.worldID, L"worldID",
					player.getInformation().playerId, L"playerID",
					player.getInformation().name, L"username");

				access->sendMessage(Net::Client::ClassicSimulatorMessageID::SimulationFailure);
				callback->onSimulationFailure();

				return;
			}

			if (simulation.getTick() < synchronizeBeginTick)
			{
				Log::Warning(L"Got invalid tick for synchronization",
					simulation.getTick(), L"new_tick",
					synchronizeBeginTick, L"prepared_tick",
					info.representationID, L"reprID",
					info.worldID, L"worldID",
					player.getInformation().playerId, L"playerID",
					player.getInformation().name, L"username");

				synchronizingFrames.clear();

				return;
			}

			if (simulation.getTick() - synchronizeBeginTick != 0)
			{
				Log::Information(L"true, this can be not zero [ccsh]",
					simulation.getTick(), L"new_tick",
					synchronizeBeginTick, L"beg_tick",
					synchronizingFrames.size(), L"buffered");
			}

			Log::Information(L"before", player.injectedFrames.size(), L"injected");

			// skipping all frames that the simulator received
			for (int i = simulation.getTick() - synchronizeBeginTick; i < synchronizingFrames.size(); ++i)
			{
				player.inject(synchronizingFrames[i]);
				
				ClassicSimulation::WorldFailure result = simulation.processLogic();
				if (result != ClassicSimulation::WorldFailure::Success)
				{
					access->sendMessage(Net::Client::ClassicSimulatorMessageID::SimulationFailure);
					callback->onSimulationFailure();

					Log::Error(L"Failed to process simulation",
						(int)result, L"result");
					
					break;
				}
			}

			Log::Information(L"after", player.injectedFrames.size(), L"injected",
				simulation.getTick(), L"new_tick",
				synchronizeBeginTick, L"prepared_tick",
				info.representationID, L"reprID",
				info.worldID, L"worldID",
				player.getInformation().playerId, L"playerID",
				player.getInformation().name, L"username");

			synchronizingFrames.clear();
		}

		bool processLogic()
		{
			const ClassicSimulation::WorldFailure result = simulation.processLogic();

			if (result != ClassicSimulation::WorldFailure::Success)
			{
				access->sendMessage(Net::Client::ClassicSimulatorMessageID::SimulationFailure);
				callback->onSimulationFailure();

				Log::Error(L"Failed to process simulation",
					(int)result, L"result");

				return false;
			}
			else
			{
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
	};
}
