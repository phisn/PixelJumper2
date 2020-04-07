#pragma once

#include <Client/source/device/InputDevice.h>
#include <Client/source/scene/GameBaseScene.h>

// #include <Client/source/game/tile/GameWallTile.h>

namespace Scene
{
	/*
	struct LocalGameSettings
	{
		Game::WorldSettings* world;

		int playerCount;
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
				settings->world),
			settings(settings)
		{
		}

		~LocalGame()
		{
			delete settings;
		}

		bool onCreate() override
		{
			if (settings->playerCount <= 0 ||
				settings->playerCount > 4)
			{
				return false;
			}

			return GameBase::onCreate();
		}

		void onRemove() override
		{
			GameBase::onRemove();

			for (GAME::LocalPlayer* player : localPlayers)
			{
				delete player;
			}
		}

		void initialize() override
		{
			GameBase::initialize();

			GAME::PlayerSettings playerSettings;
			for (int i = 0; i < settings->playerCount; ++i)
			{
				switch (i)
				{
				case 0:
					playerSettings.color = sf::Color::Red;
					playerSettings.name = L"Player 1";

					break;
				case 1:
					playerSettings.color = sf::Color::Blue;
					playerSettings.name = L"Player 2";

					break;
				case 2:
				case 3:
					break;
				}

				GAME::LocalPlayer* localPlayer = new GAME::LocalPlayer(
					playerSettings, 
					DEVICE::Interface::GetInput()->loadLocalInput(i)
				);

				localPlayer->getView()->adjustView(
					i, settings->playerCount);

				world->initializePlayer(
					localPlayer);
				localPlayers.push_back(localPlayer);
			}
		}

		void onShow() override { }
		void onHide() override { }

		void onLogic(
			const sf::Time time) override
		{
			GameBase::onLogic(time);

			for (GAME::LocalPlayer* localPlayer : localPlayers)
			{
				localPlayer->onLogic(time);

				world->updatePlayer(
					localPlayer,
					time);
			}
		}

		void onDraw() override
		{
			for (GAME::LocalPlayer* localPlayer : localPlayers)
			{
				localPlayer->getView()->apply();

				GameBase::onDraw();

				for (GAME::LocalPlayer* player : localPlayers)
				{
					player->onDraw();
				}

				tryDrawForeground();
			}
		}

	private:
		std::vector<
			GAME::LocalPlayer*> localPlayers;

		LocalGameSettings* settings;
	};
	*/
}
