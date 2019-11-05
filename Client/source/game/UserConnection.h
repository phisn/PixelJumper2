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
		enum Type
		{
			Local,
			Remote,
			Ghost
		};

		UserConnection(
			const Type type,
			const PlayerInformation info)
			:
			type(type),
			information(info)
		{
		}

		virtual void initialize() = 0;
		virtual void onLogic(const sf::Time time) = 0;

		virtual void changeUserInformation(const PlayerInformation info)
		{
			information = info;
		}

		virtual void pushPlayer(World* const world)
		{
			world->addPlayer(getPlayer());
		}

		virtual void popPlayer(World* const world)
		{
			world->removePlayer(getPlayer());
		}

		const PlayerInformation& getInformation() const
		{
			return information;
		}

		const Type type;
	protected:
		virtual PlayerBase* getPlayer() = 0;

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
			const Device::Input::PlayerId playerId,
			const sf::FloatRect viewPort)
			:
			UserConnection(Local, info),
			playerId(playerId)
		{
			view.setViewport(viewPort);
			player = new LocalPlayer(info);
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

			// player should never be null
			delete player;
			player = new LocalPlayer(info);
		}

		LocalPlayer* getLocalPlayer() const
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

		LocalPlayer* player = NULL;

		const Device::Input::PlayerId playerId;
		Device::View view;
	};

	class RemoteConnection
		:
		public UserConnection
	{
	public:
		struct Message
		{
			Resource::PlayerId playerId;

			enum Type
			{
				Sync,
				Frame
			} type;

			sf::Packet packet;
		};

		RemoteConnection(
			const sf::IpAddress ipAddress,
			const PlayerInformation info)
			:
			ipAddress(ipAddress),
			UserConnection(Remote, info)
		{
			player = new VirtualPlayer(info);
		}

		void initialize() override
		{
		}

		void onLogic(const sf::Time time) override
		{
		}

		void changeUserInformation(const PlayerInformation info) override
		{
			UserConnection::changeUserInformation(info);

			// player should never be null
			delete player;
			player = new VirtualPlayer(info);
		}

		VirtualPlayer* getVirtualPlayer() const
		{
			return player;
		}

		const sf::IpAddress& getIpAddress() const
		{
			return ipAddress;
		}

	protected:
		PlayerBase* getPlayer() override
		{
			return player;
		}

		const sf::IpAddress ipAddress;
		VirtualPlayer* player;
	};

	class GhostConnection
	{
	public:

	};
}
