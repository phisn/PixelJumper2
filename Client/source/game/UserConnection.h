#pragma once

#include <Client/source/game/GamePlayer.h>
#include <Client/source/game/GameWorld.h>
#include <Client/source/game/LocalPlayer.h>

namespace Game
{
	class UserConnection
	{
	public:
		virtual void initialize() = 0;
		virtual void createPlayer(World* const world) = 0;

		const PlayerInformation& getInformation() const
		{
			return information;
		}

	protected:
		PlayerInformation information;
	};

	class LocalConnection
		:
		public UserConnection
	{
	public:
		LocalConnection(
			const PlayerInformation info,
			const Device::Input::PlayerId playerId,
			const sf::FloatRect viewPort)
			:
			playerId(playerId)
		{
			view.setViewport(viewPort);
			information = info;
		}

		void initialize() override
		{
		}

		void createPlayer(World* const world) override
		{
			world->addPlayer(
				currentPlayer = new LocalPlayer(playerId, information, &view)
			);
		}

		void enableView(sf::RenderTarget* const target)
		{
			target->setView(view);
		}

	private:
		LocalPlayer* currentPlayer = NULL;
		const Device::Input::PlayerId playerId;
		Device::View view;
	};
}
