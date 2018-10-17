#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/InputDevice.h>

#include <Client/source/game/LocalPlayer.h>
#include <Client/source/game/PlayerBase.h>
#include <Client/source/game/World.h>

#include <Client/source/scene/SceneBase.h>
#include <Client/source/scene/SceneInterface.h>

namespace Scene
{
	class GameBase
		:
		public Base
	{
	public:
		GameBase(
			Game::World* world)
			:
			Base(true, Scene::Type::SFML)
		{
		}

		void onCatch(
			FallbackType type) override
		{
			if (type == FallbackType::Default)
			{
				onShow();
			}
			else
			{
				SCENE::Interface::fallback(type);
			}
		}

		void onEvent(
			sf::Event event) override
		{
			if (event.type == sf::Event::KeyPressed)
			{
				switch (
					DEVICE::Interface::getInput()->codeToSymbol(event.key.code)
					)
				{
				case DEVICE::GlobalInputSymbol::Exit:
					SCENE::Interface::popScene();

					break;
				case DEVICE::GlobalInputSymbol::OpenMenu:
					// SCENE::Interface::pushScene(
					//     new GameMenu());

					break;
				case DEVICE::GlobalInputSymbol::Pause:
					// ...

					break;
				case DEVICE::GlobalInputSymbol::Invalid:
				default:
					break;
				}
			}

			return;
		}

		void onLogic(
			sf::Time time) override
		{
			world->onLogic(time); // ?correct position?

			for (GAME::LocalPlayer* localPlayer : localPlayers)
			{
				localPlayer->onLogic(time);

				world->updatePlayer(
					localPlayer, time);
			}
		}

		void onDraw() override
		{
			for (GAME::LocalPlayer* playerView : localPlayers)
			{
				playerView->getView()->apply();
				world->onDraw();

				// ...

				for (const GAME::LocalPlayer* localPlayer : localPlayers)
				{
					localPlayer->onDraw();
				}
			}
		}
	protected:
		std::vector<Game::LocalPlayer*> localPlayers;

		Game::World* world;
	};
}
