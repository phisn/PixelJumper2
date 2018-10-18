#pragma once

#include <Client/source/scene/GameBaseScene.h>

namespace Scene
{
	struct LocalGameSettings
	{
		Game::WorldSettings world;

		int playerCount;
	};

	class LocalGame
		:
		GameBase
	{
	public:
		LocalGame(
			LocalGameSettings* settings)
			:
			GameBase(
				&settings->world),
			settings(settings)
		{
		}

		bool onCreate() override
		{
			if (settings->playerCount <= 0 ||
				settings->playerCount > 4)
			{
				return false;
			}

			if (!GameBase::onCreate())
			{
				return false;
			}
		}

		void initialize() override
		{
			GameBase::initialize();

			for (int i = 0; i < settings->playerCount; ++i)
			{


				world->initializePlayer(
					localPlayers.back());
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

	class _LocalGame
		:
		public GameBase
	{
	public:
		_LocalGame(
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
