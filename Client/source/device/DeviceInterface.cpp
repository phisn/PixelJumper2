#include <Client/source/device/ScreenDevice.h>
#include <Client/source/device/InputDevice.h>
#include <Client/source/device/ResourceDevice.h>

#include <Client/source/scene/SceneInterface.h>

#include <Client/source/scene/LocalGameScene.h>
#include <Client/source/scene/Context.h>

#include <Client/source/game/tiles/TileManager.h>
#include <Client/source/game/tiles/Wall.h>

#include "DeviceInterface.h"

#include <Client/source/game/WorldManager.h>

namespace
{
	Device::GlobalInput* input = NULL;
	Device::Screen* screen = NULL;
	Device::Resource* resource = NULL;

#ifdef _DEBUG
	SCENE::Context* makeStartingContext()
	{
		GAME::Tile::Manager::registerAllPrivate();

		SCENE::LocalGameSettings* settings = new SCENE::LocalGameSettings();
		// settings->world = new GAME::WorldSettings();

		settings->playerCount = 2;

		/*
		settings->world->author = L"Phisn";
		settings->world->name = L"TestMap";

		settings->world->size = sf::Vector2f(
			40.f, 18.f);

		settings->world->tiles.push_back(
			new GAME::Tile::Wall(
				{
					sf::Vector2f(16.f, 2.f),
					sf::Vector2f(2.f, 14.f)
				})); // wall

		settings->world->tiles.push_back(
			new GAME::Tile::Wall(
				{
					sf::Vector2f(1.f, 10.f),
					sf::Vector2f(16.f, 2.f)
				})); // floor

		std::cout << GAME::WorldManager::saveSettingsToFile(
			L"defaultWorld.pxjmpr",
			settings->world) << std::endl;
		*/

		settings->world = GAME::WorldManager::loadSettingsFromFile(
			L"defaultWorld.pxjmpr");

		std::cout << (int)settings->world << std::endl;

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
