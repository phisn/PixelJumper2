#pragma once

#include <Client/source/scene/GameBaseScene.h>

namespace Scene
{
	struct LocalGameSettings
	{
		Game::WorldSettings worldSettings;

		int playerCount;
		std::vector<Game::PlayerSettings> playerSettings;
	};

	class LocalGame
		:
		public GameBase
	{
	public:
		LocalGame(
			LocalGameSettings* settings)
			:
			GameBase(
				new GAME::World()
			),
			settings(settings)
		{
		}

		bool onCreate() override
		{
			if (!world->initialize(
					&settings->worldSettings))
			{
				return false;
			}

			localPlayers.resize(settings->playerCount);
			for (int i = 0; i < settings->playerCount; ++i)
			{
				GAME::LocalPlayer* player = new GAME::LocalPlayer(
					settings->playerSettings[i],
					DEVICE::Interface::getInput()->loadLocalInput(i)
				);

				player->getView()->adjustView(
					i, settings->playerCount);

				world->initializePlayer(player);
				localPlayers.push_back(player);
			}

			return true;
		}

		void onRemove() override
		{
			for (GAME::LocalPlayer* player : localPlayers)
			{
				delete player;
			}

			delete world;
		}

		void onShow() override
		{
		}

		void onHide() override
		{
		}
	private:
		LocalGameSettings* settings;
	};
}
