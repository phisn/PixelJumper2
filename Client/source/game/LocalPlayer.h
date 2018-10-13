#pragma once

#include <Client/source/game/GameView.h>
#include <Client/source/game/PlayerBase.h>

#define IF_HAS_ACTION(source, except) if ((int) source & (int) except)

namespace Game
{
	struct PlayerProperties
	{
		sf::Vector2f movement;
		bool isOnGround;
	};

	class LocalPlayer
		:
		public PlayerBase
	{
	public:
		enum class Action
		{
			ChangeView
		};

		LocalPlayer();
		~LocalPlayer();

		void onAction(
			const Action action)
		{

		}

		void onLogic(
			const sf::Time time)
		{

		}

		PlayerProperties* changeProperties()
		{
			return &properties;
		}

		const PlayerProperties* getProperties() const
		{
			return &properties;
		}

		View* getView()
		{
			return &view;
		}
	private:
		PlayerProperties properties;
		Action action;

		Game::View view;
	};
}
