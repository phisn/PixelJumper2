#include <Client/source/device/ScreenDevice.h>
#include <Client/source/device/InputDevice.h>
#include <Client/source/device/ResourceDevice.h>

#include <Client/source/scene/SceneInterface.h>

#include <Client/source/scene/LocalGameScene.h>
#include <Client/source/scene/Context.h>
#include <Client/source/game/tiles/Wall.h>

#include "DeviceInterface.h"

namespace
{
	Device::GlobalInput* input = NULL;
	Device::Screen* screen = NULL;
	Device::Resource* resource = NULL;

#ifdef _DEBUG
	SCENE::Context* makeStartingContext()
	{
		SCENE::LocalGameSettings* settings = new SCENE::LocalGameSettings();

		settings->playerCount = 2;

		settings->world.begin = sf::Vector2f(5.f, 0.f);
		settings->world.size  = sf::Vector2f(40.f, 18.f);
		settings->world.speed = 0.00001f;

		GAME::Tile::Wall* wall1 = new GAME::Tile::Wall(
			{
				sf::Vector2f(1.f, 10.f),
				sf::Vector2f(16.f, 2.f)
			});

		settings->world.tiles.push_back(
			wall1); // floor

		GAME::Tile::Wall* wall2 = new GAME::Tile::Wall(
			{
				sf::Vector2f(16.f, 2.f),
				sf::Vector2f(2.f, 14.f)
			});

		settings->world.tiles.push_back(
			wall2); // wall

		return SCENE::Context::create<
			SCENE::LocalGame>(settings);
	}
#else
	SCENE::Context* makeStartingContext()
	{

	}
#endif
}

namespace Device
{
	InitError Interface::initialize()
	{
		if (input)
		{
			delete input;
		}

		if (screen)
		{
			delete screen;
		}

		if (resource)
		{
			delete resource;
		}

		input = new GlobalInput();
		screen = new Screen();
		resource = new Resource();

		if (!input->initialize())
		{
			return InitError::Input;
		}

		if (!screen->initalize())
		{
			return InitError::Scene;
		}

		if (!SCENE::Interface::pushContext(
			makeStartingContext()
		))
		{
			return InitError::Scene;
		}

		// Net

		if (!resource->initialize())
		{
			return InitError::Resource;
		}

		return InitError::Invalid;
	}

	int Interface::start()
	{
		screen->onShow();

		sf::Clock clock;
		sf::Event event;

		while (true)
		{
			Scene::Interface::doOrders();

			if (!Scene::Interface::isRunning())
			{
				break;
			}

			while (screen->getWindow()->pollEvent(event))
			{
				Scene::Interface::onEvent(event);
			}

			Scene::Interface::onUpdate(
				clock.restart());

			screen->getWindow()->clear();
			Scene::Interface::onDraw();
			screen->getWindow()->display();
		}

		return 0;
	}

	GlobalInput* Interface::getInput()
	{
		return input;
	}

	Screen* Interface::getScreen()
	{
		return screen;
	}
	Resource* Interface::getResource()
	{
		return nullptr;
	}
}
