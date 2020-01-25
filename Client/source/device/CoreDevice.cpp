#include <Client/source/device/DeviceConfig.h>
#include <Client/source/device/InputDevice.h>
#include <Client/source/device/RandomDevice.h>
#include <Client/source/device/ScreenDevice.h>

#include "CoreDevice.h"

#include <Client/source/framework/Context.h>
#include <Client/source/framework/FrameworkInterface.h>

#include <Client/source/shared/tiles/TileDescription.h>

#include <Client/source/logger/Logger.h>

#include <Client/source/resource/ResourceInterface.h>

#include <Client/source/scene/EditorScene.h>
// #include <Client/source/scene/TestGameScene.h>
// #include <Client/source/scene/TestMenuScene.h>
// #include <Client/source/scene/TestReleaseGameScene.h>
// #include <Client/source/scene/GameJoinScene.h> 

namespace Device
{
	bool PushMainContext()
	{
		return Framework::Context::Push<Scene::TestGameScene>();
	}

	Core::Error Core::Initialize()
	{
		Log::Section section(L"Initializing Game");

		Shared::TileDescription::Initialize();

		if (!Resource::Interface::Initialize())
		{
			return Core::Error::ResourceError;
		}

		if (!Config::Initialize())
		{
			return Core::Error::SettingsError;
		}

		Random::Initialize();

		if (!Screen::Initialize())
		{
			return Core::Error::ScreenError;
		}
		
		if (!Input::Initialize())
		{
			return Core::Error::InputError;
		}

		return Core::Error::Success;
	}

	void Core::Uninitialize()
	{
		Input::Uninitialize();
		Screen::Uninitialize();

		Config::Uninitialize();
		Resource::Interface::Uninitalize();
	}

	int Core::RunGameLoop()
	{
		Screen::_CreateWindow();

		sf::Clock clock;
		sf::Event event;

		// Context must be pushed in RunGameLoop
		// (after creating window) and not in Initialize
		// because internal initializations inside
		// contexts use window properties like size

		Log::Information(L"Pushing main context");
		if (!PushMainContext())
		{
			Log::Error(L"Failed to create starting context");

			return 1;
		}

		Log::Information(L"Entering game loop");
		while (true)
		{
			Framework::Interface::ProcessTask();

			if (!Framework::Interface::IsRunning())
			{
				break;
			}

			while (Device::Screen::PollEvent(event))
			{
				Framework::Interface::Event(event);
			}

			Framework::Interface::Update(clock.restart());

			Device::Screen::BeginDraw();
			Framework::Interface::Draw(Device::Screen::GetTarget());
			Device::Screen::EndDraw();
		}

		return 0;
	}
}
