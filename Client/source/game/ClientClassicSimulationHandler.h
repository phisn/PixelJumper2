#pragma once

#include <Client/source/game/CachedControllablePlayer.h>
#include <Client/source/game/ClassicSimulation.h>
#include <Client/source/game/net/ClassicSimulatorMessage.h>
#include <Client/source/net/RequestHandlerBase.h>

#include <deque>

namespace Game::Net
{
	class ArtificialPlayer
	{
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
			representation->update(time);
		}

		void processLogic()
		{
			if (++procesLogicCounter > NetMovementCaptureTime && movement.size() > 0)
			{
				representation->setPosition(movement.front());
				movement.pop_front();

				procesLogicCounter = 0;
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
		}

		const PlayerInformation& getInformation() const
		{
			return playerInfo;
		}

	private:
		std::deque<sf::Vector2f> movement;

		size_t procesLogicCounter = 0;

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
				Device::Input::PlayerID::P1,
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

		virtual void onDraw(sf::RenderTarget* const target)
		{
			simulation.draw(target);

			for (ArtificialPlayer* const artificialPlayer : artificialPlayers)
				artificialPlayer->draw(target);

			player.onDraw(target);
		}

		void update() override
		{
			Client::PushMovementMessage message;
			message.packetFrameStatus = &player.packedFrameStatus;
			
			access->accessSendMessage(
				Client::ClassicSimulatorMessageID::PushMovement,
				&message);

			player.packedFrameStatus.frames.clear();
		}

		bool onMessage(
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Host::ClassicSimulatorMessageID::PushPlayer:
				if (Host::PushPlayerMessage message; loadMessage(messageID, &message, pipe))
				{
					onPushPlayer(message);
				}

				return true;
			case Host::ClassicSimulatorMessageID::PopPlayer:
				if (Host::PopPlayerMessage message; loadMessage(messageID, &message, pipe))
				{
					onPopPlayer(message);
				}

				return true;
			case Host::ClassicSimulatorMessageID::PushMovement:
			{
				NetPlayerMovement movement;
				Host::PlayerMovementMessage message;
				message.movement = &movement;

				if (loadMessage(messageID, &message, pipe))
				{
					onPlayerMovement(message);
				}

				return true;
			}
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

		void onPushPlayer(const Host::PushPlayerMessage& message)
		{
			artificialPlayers.push_back(
				new ArtificialPlayer(message.playerID, message.representationID, "")
			);
		}

		void onPopPlayer(const Host::PopPlayerMessage& message)
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

		void onPlayerMovement(const Host::PlayerMovementMessage& message)
		{
			// if (message.movement->content.worldID != simulation.)

			for (ArtificialPlayer* const artificialPlayer : artificialPlayers)
				if (artificialPlayer->getInformation().playerId == message.movement->content.playerID)
				{
					artificialPlayer->pushMovement(message.movement);
					break;
				}
		}
	};
}
