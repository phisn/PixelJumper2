#pragma once

#include <Client/source/game/GamePlayer.h>
#include <Client/source/game/GameWorld.h>

#include <Client/source/game/ControllablePlayer.h>
#include <Client/source/game/VirtualPlayer.h>

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

		virtual void initialize() = 0;

		virtual void onLogic(const sf::Time time) = 0;

		// local: process input and store if
		// connection goes to remote host
		// remote: process new input and determine
		// timeouts and eventuelly change status
		// return: is currently able to 
		// process logic
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

		void initialize() override
		{
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

	class RemoteConnection
		:
		public UserConnection
	{
	public:
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

		struct Settings
		{
			sf::Time softTimeout = sf::milliseconds(500);	// allowed lag
			int hardTimeoutCount = 4; // 2 sec				// timout after N lags
		};

		// status should be used by the connection
		// itself and the operator. if the status
		// changes from connected, then the connection
		// has to be removed

		// order by importance
		enum Status
		{
			Connected,
			Disconnected,
			Timeout,
			Error
		};

		RemoteConnection(
			const sf::IpAddress ipAddress, // change to (socket context)
			const Settings settings,
			const PlayerInformation info)
			:
			ipAddress(ipAddress),
			settings(settings),
			UserConnection(info)
		{
			player = new VirtualPlayer(info);
		}

		void initialize() override
		{
		}

		void onLogic(const sf::Time time) override
		{
			Net::FrameStatus fs;
		}

		void processLogic() override
		{
			if (!player->hasUpdate())
			{

			}
		}

		void changeUserInformation(const PlayerInformation info) override
		{
			UserConnection::changeUserInformation(info);

			// player should never be null
			delete player;
			player = new VirtualPlayer(info);
		}

		void adjustStatus(const Status status)
		{
			if (this->status < status)
				this->status = status;
		}

		Status getStatus() const
		{
			return status;
		}

		PlayerBase* getPlayer() override
		{
			return player;
		}

		const sf::IpAddress& getIpAddress() const
		{
			return ipAddress;
		}

	protected:
		const sf::IpAddress ipAddress;
		const Settings settings;
		
		Status status;
		VirtualPlayer* player;
	};

	class GhostConnection
	{
	public:

	};
}
