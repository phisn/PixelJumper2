#pragma once

#include "CachedControllablePlayer.h"

#include "GameCore/ClassicSimulation.h"
#include "GameCore/net/ClassicSimulatorMessage.h"
#include "NetCore/RequestHandler.h"

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
	};

	class ClientClassicSimulationHandler
		:
		public ::Net::RequestHandler
	{
	public:
		ClientClassicSimulationHandler(
			ClientClassicSimulationHandlerCallback* const callback,
			const SimulationBootInformation info,
			const WorldResourceContainer& worldContainer,
			Device::View* const view)
			:
			callback(callback),
			info(info),
			worldContainer(worldContainer),
			player(
				1,
				PlayerInformation{
					0,
					info.representationID,
					""
				},
				view),
			simulation(worldContainer)
		{
			simulation.setPlayer(&player);
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

					processLogic();

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
					processLogic();
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
			Net::Client::PushMovementMessage message;
			message.packetFrameStatus = &player.packedFrameStatus;
			
			access->sendMessage(
				Net::Client::ClassicSimulatorMessageID::PushMovement,
				&message);

			player.packedFrameStatus.frames.clear();
		}

		bool onMessage(
			const Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
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
			}

			return false;
		}

	protected:
		virtual void onSimulationStarted() = 0;
		virtual void onSimulationFailed(const ClassicSimulation::WorldFailure reason) = 0;

	private:
		ClientClassicSimulationHandlerCallback* const callback;
		const SimulationBootInformation info;
		const WorldResourceContainer& worldContainer;

		VisualClassicSimulation simulation;
		CachedControllablePlayer player;

		std::vector<ArtificialPlayer*> artificialPlayers;

		sf::Uint64 logicCounter = 0;

		// used to remove tick indifference with server
		bool gameSpeedAdjusted = false;
		float currentGameSpeed;
		sf::Uint64 remainingGameSpeedAdjustment;

		void onPushPlayer(const Net::Host::PushPlayerMessage& message)
		{
			artificialPlayers.push_back(
				new ArtificialPlayer(message.playerID, message.representationID, "")
			);
		}

		void onPopPlayer(const Net::Host::PopPlayerMessage& message)
		{
			decltype(artificialPlayers)::iterator iterator = std::find_if(
				artificialPlayers.begin(),
				artificialPlayers.end(),
				[&message](ArtificialPlayer* const artificialPlayer)
				{
					return artificialPlayer->getInformation().playerId == message.playerID;
				});

			if (iterator != artificialPlayers.end())
				artificialPlayers.erase(iterator);
		}

		void onPlayerMovement(const Net::Host::PlayerMovementMessage& message)
		{
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
			currentGameSpeed = message.speedAdjustment;
			gameSpeedAdjusted = true;
			remainingGameSpeedAdjustment = message.speedAdjustmentLength;

			Log::Information(L"speed adjusted",
				currentGameSpeed, L"speed",
				remainingGameSpeedAdjustment, L"length");
		}

		void processLogic()
		{
			const ClassicSimulation::WorldFailure result = simulation.processLogic();

			if (result != ClassicSimulation::WorldFailure::Success)
			{
				// call parent virtual pure

				Log::Error(L"Failed to process simulation",
					(int)result, L"result");
			}

			for (ArtificialPlayer* const artificialPlayer : artificialPlayers)
			{
				artificialPlayer->processLogic();
				artificialPlayer->process(sf::microseconds(LogicTimeStep));
			}
		}
	};
}
