#pragma once

#include <Client/source/game/GamePlayer.h>
#include <Client/source/game/GameWorld.h>

#include <Client/source/game/ControllablePlayer.h>
#include <Client/source/game/VirtualPlayer.h>

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

		UserConnection(const Type type)
			:
			type(type)
		{
		}

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

		virtual PlayerBase* getPlayer() = 0;

		virtual void changeUserInformation(const PlayerInformation info)
		{
			information = info;
		}

		const PlayerInformation& getInformation() const
		{
			return information;
		}

		const Type type;
	protected:
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
			UserConnection(info),
			playerId(playerId)
		{
			view.setViewport(viewPort);
			information = info;

			player = new LocalPlayer(info);
		}

		~LocalConnection()
		{
			delete player;
		}

		void initialize() override
		{
		}

		PlayerBase* getPlayer() override
		{
			return player;
		}

		void changeUserInformation(const PlayerInformation info) override
		{
			UserConnection::changeUserInformation(info);

			// player should never be null
			delete player;
			player = new LocalPlayer(info);
		}

		void enableView(sf::RenderTarget* const target)
		{
			target->setView(view);
		}

	private:
		LocalPlayer* player = NULL;

		const Device::Input::PlayerId playerId;
		Device::View view;
	};

	class RemoteConnection
		:
		public UserConnection
	{
	public:
		void initialize() override
		{
		}

		void onLogic(const sf::Time time) override
		{
		}

		virtual PlayerBase* getPlayer()
		{
		}

	private:
		VirtualPlayer* virtualPlayer;
	};

	class GhostConnection
	{
	public:

	};
}
