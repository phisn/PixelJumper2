#pragma once

#include <Client/source/scene/GameBaseScene.h>

namespace Scene
{
	class LocalGame
		:
		public GameBase
	{
	public:
		LocalGame(
			Game::World* world,
			int localPlayerCount)
			:
			GameBase(world)
		{
			for (int i = 0; i < localPlayerCount; ++i)
			{
				GAME::PlayerSettings settings;
				DEVICE::LocalInputSettings input;

				switch (localPlayerCount)
				{
				case 0:
					settings.color = sf::Color::Red;
					settings.name = L"Player 1";

					break;
				case 1:
					settings.color = sf::Color::Blue;
					settings.name = L"Player 2";

					break;
				case 2:
					break;
				case 3:
					break;
				}

				localPlayers.emplace_back(
					settings,
					DEVICE::Interface::getInput()->loadLocalInput(i)
				);

				localPlayers.back()->getView()->adjustView(
					i, localPlayerCount);
			}
		}

		bool onCreate() override
		{

		}

		void onRemove() override
		{

		}

		void onShow() override
		{

		}

		void onHide() override
		{

		}
	};
}
