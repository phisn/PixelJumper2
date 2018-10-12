#pragma once

#include <Client/source/game/GameView.h>
#include <Client/source/game/PlayerBase.h>

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
			Trigger,

			Up,
			Down,
			Left,
			Right
		};

		LocalPlayer();
		~LocalPlayer();

		void onAction(
			const Action action)
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

		Game::View view;
	};
}
