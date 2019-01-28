#include <Client/source/device/ScreenDevice.h>
#include <Client/source/device/InputDevice.h>
#include <Client/source/device/RandomDevice.h>
#include <Client/source/device/ResourceDevice.h>

#include "DeviceInterface.h"

#include <Client/source/framework/Context.h>
#include <Client/source/framework/FrameworkInterface.h>

#include <Client/source/game/tiles/TileManager.h>
#include <Client/source/game/tiles/Wall.h>

#include <Client/source/logger/Logger.h>

#include <Client/source/scene/EditorScene.h>
#include <Client/source/scene/LocalGameScene.h>


namespace
{
	Device::GlobalInput* input = NULL;
	Device::Screen* screen = NULL;
	Device::Resource* resource = NULL;

#ifdef _DEBUG
	FW::Context* makeStartingContext()
	{
		return FW::Context::create<SCENE::Editor>();

		/*
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

		settings->world = GAME::WorldManager::loadSettingsFromFile(
			L"defaultWorld.pxjmpr");

		std::cout << (int)settings->world << std::endl;
		*/
	}
#else
	FW::Context* makeStartingContext()
	{

	}
#endif
}

namespace Device
{
	InitError Interface::Initialize()
	{
		Random::Initialize();

		Log::Section section(L"Initializing Game");

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

		if (!resource->initialize())
		{
			return InitError::Resource;
		}

		return InitError::Invalid;
	}

	int Interface::Start()
	{
		screen->onShow();

		sf::Clock clock;
		sf::Event event;

		if (!FW::Interface::PushContext(
			makeStartingContext()
		))
		{
			Log::Error(L"Failed to create starting context");

			return 0;
		}

		Log::Information(L"Entering game loop");
		while (true)
		{
			Framework::Execution::DoTasks();

			if (!Framework::Execution::IsRunning())
			{
				break;
			}

			while (screen->getWindow()->pollEvent(event))
			{
				Framework::Execution::OnEvent(event);
			}

			Framework::Execution::OnUpdate(
				clock.restart()
			);

			screen->getWindow()->clear();
			Framework::Execution::OnDraw();
			screen->getWindow()->display();
		}

		return 0;
	}

	GlobalInput* Interface::GetInput()
	{
		return input;
	}

	Screen* Interface::GetScreen()
	{
		return screen;
	}

	Resource* Interface::GetResource()
	{
		return nullptr;
	}
}
