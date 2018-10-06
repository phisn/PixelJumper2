#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/InputDevice.h>

#include <Client/source/game/GameInterface.h>

#include <Client/source/scene/SceneBase.h>
#include <Client/source/scene/SceneInterface.h>

namespace Scene
{
	class Game
		:
		public Base
	{
	public:
		Game()
			:
			Base(true, Scene::Type::SFML)
		{
		}

		bool onCreate() override
		{
			return GAME::Interface::initialize();
		}

		void onRemove() override
		{
			GAME::Interface::reset();
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
			if (IS_ONLINE(
					GAME::Interface::getType() ))
			{

			}

			GAME::Interface::getWorld()->onLogic(time);

			for (GAME::LocalPlayer* localPlayer : GAME::Interface::getLocalPlayers())
			{
				localPlayer->onLogic(time);
			}
		}

		void onDraw() override
		{
			for (GAME::LocalPlayer* playerView : GAME::Interface::getLocalPlayers())
			{
				playerView->getView()->apply();

				GAME::Interface::getWorld()->draw();

				for (const GAME::LocalPlayer* localPlayer : GAME::Interface::getLocalPlayers())
				{
					localPlayer->draw();
				}
			}
		}
	};
}
