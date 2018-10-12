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
			for (GAME::LocalPlayer* localPlayer : GAME::Interface::getLocalPlayers())
			{
				localPlayer->onEvent(event);
			}

			if (event.type == sf::Event::KeyPressed)
			{
				switch (
					DEVICE::Interface::getInput()->codeToSymbol(
						event.key.code))
				{
				case DEVICE::InputSymbol::Exit:
					SCENE::Interface::popScene();

					break;
				case DEVICE::InputSymbol::OpenMenu:
					// SCENE::Interface::pushScene(
					//     new GameMenu());

					break;
				case DEVICE::InputSymbol::Pause:
					// ...

					break;
				case DEVICE::InputSymbol::Invalid:
				default:
					break;
				}
			}

			return;
		}

		void onLogic(
			sf::Time time) override
		{
			world->onLogic(time);

			for (GAME::LocalPlayer* localPlayer : localPlayers)
			{
				world->updatePlayer(
					localPlayer, time);
			}
		}

		void onDraw() override
		{
			for (GAME::LocalPlayer* playerView : localPlayers)
			{
				playerView->getView()->apply();
				world->draw();

				// ...

				for (const GAME::LocalPlayer* localPlayer : localPlayers)
				{
					localPlayer->draw();
				}
			}
		}
	private:
		std::vector<Game::LocalPlayer*> localPlayers;

		Game::World* world;
	};
}
