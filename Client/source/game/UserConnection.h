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

}
