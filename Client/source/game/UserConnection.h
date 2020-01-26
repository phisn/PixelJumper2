#pragma once

#include <Client/source/game/ControllablePlayer.h>
#include <Client/source/game/Simulation.h>
#include <Client/source/game/VirtualPlayer.h>

#include <Client/source/resource/pipes/PacketPipe.h>

#include <SFML/Network.hpp>

namespace Game
{
	class UserConnection
	{
	public:
		UserConnection(const PlayerInformation info)
			:
			information(info)
		{
		}

		virtual bool initialize() = 0;

		virtual void onLogic(const sf::Time time) = 0;
		virtual bool processLogic() = 0;

		virtual void changeUserInformation(const PlayerInformation info)
		{
			information = info;
		}

		const PlayerInformation& getInformation() const
		{
			return information;
		}

		virtual PlayerBase* getPlayer() = 0;

	private:
		PlayerInformation information;
	};

	class LocalConnection
		:
		public UserConnection
	{
	public:
		// playerinfo is normally available for
		// a local connection
		LocalConnection(
			const PlayerInformation info,
			const Device::Input::PlayerID playerId,
			const sf::FloatRect viewPort)
			:
			UserConnection(info),
			playerId(playerId)
		{
			view.setViewport(viewPort);
			player = new ControllablePlayer(
				playerId, 
				info, 
				&view);
		}

		~LocalConnection()
		{
			delete player;
		}

		bool initialize() override
		{
			return true;
		}

		void changeUserInformation(const PlayerInformation info) override
		{
			UserConnection::changeUserInformation(info);

			// player must not be null
			delete player;
			player = new ControllablePlayer(playerId, info, &view);
		}

		void onLogic(const sf::Time time) override
		{
			player->onLogic(time);
		}

		bool processLogic() override
		{
			// TODO: save input or create new localplayer variation

			return true;
		}

		ControllablePlayer* getLocalPlayer() const
		{
			return player;
		}

		void enableView(sf::RenderTarget* const target)
		{
			target->setView(view);
		}

	private:
		PlayerBase* getPlayer() override
		{
			return player;
		}

		ControllablePlayer* player;
		Device::View view;
		const Device::Input::PlayerID playerId;
	};

	struct ConnectionContext
	{
	};

	class RemoteConnection
	{
	public:
		/*
			struct Message
			{
				Resource::PlayerID playerId;

				enum Type
				{
					Sync,
					Frame
				} type;

				sf::Packet packet;
			};
		*/

		enum Status
		{
			Running,
			Pending,
			Disconnected,
			Timeout,
			Error
		};

		struct Settings
		{
			// time after how many updates the current 
			// status will cause a hard timeout
			sf::Uint32 pendingTimeout = 2;
			sf::Uint32 runningTimeout = 4;

			// tickratedifference represents the
			// difference between client tickrate and
			// the server tickrate. this tickrate
			// gets adjusted to prevent  
			int minTickrateDifference;
			int maxTickrateDifference;
		};

		RemoteConnection(const Settings settings)
			:
			settings(settings)
		{
			socket.setBlocking(false);
		}

		bool initialize()
		{
		}

		bool processLogic()
		{
			if (simulation->getStatus() == ClassicSimulation::Running)
			{
				if (player->hasUpdate())
				{
					if (simulation->processLogic())
					{
					}
				}
			}
		}

		bool update()
		{
			const sf::Socket::Status status = socket.receive(buffer);

			switch (status)
			{
			case sf::Socket::Status::Disconnected:
				adjustStatus(Disconnected);

				return false;
			case sf::Socket::Status::Done:
				handlePacket();

				break;
			case sf::Socket::Status::Error:
				adjustStatus(Error);

				return false;
			}

			return true;
		}

		Status getStatus() const
		{
			return status;
		}

		sf::TcpSocket& getSocket()
		{
			return socket;
		}

	protected:
		void handlePacket()
		{

		}

		void adjustStatus(const Status status)
		{
			if (this->status < status)
				this->status = status;
		}

		const Settings settings;

		Status status;
		
		sf::TcpSocket socket;
		sf::Packet buffer;

		ClassicSimulation* simulation = NULL;
		VirtualPlayer* player = NULL;
	};
}
