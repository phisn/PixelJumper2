#include <Client/source/device/DeviceConfig.h>
#include <Client/source/device/InputDevice.h>
#include <Client/source/device/RandomDevice.h>
#include <Client/source/device/ScreenDevice.h>

#include "CoreDevice.h"

#include <Client/source/framework/Context.h>
#include <Client/source/framework/FrameworkInterface.h>

#include <Client/source/logger/Logger.h>

#include <Client/source/resource/ResourceInterface.h>

#include <Client/source/scene/EditorScene.h>
#include <Client/source/scene/TestGameScene.h>

namespace Device
{
	Framework::Context* MakeMainContext()
	{
		return Framework::Context::Create<Scene::TestGameScene>();
	}

	Core::Error Core::Initialize()
	{
		Log::Section section(L"Initializing Game");

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
		/*
		if (!Input::Initialize())
		{
			return Core::Error::InputError;
		}
		*/

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
		if (!Framework::Interface::PushContext( MakeMainContext() ))
		{
			Log::Error(L"Failed to create starting context");

			return 1;
		}

		Log::Information(L"Entering game loop");
		while (true)
		{
			Framework::Execution::DoTasks();

			if (!Framework::Execution::IsRunning())
			{
				break;
			}

			while (Device::Screen::PollEvent(event))
			{
				Framework::Execution::OnEvent(event);
			}

			Framework::Execution::OnUpdate( clock.restart() );

			Device::Screen::BeginDraw();
			Framework::Execution::OnDraw();
			Device::Screen::EndDraw();
		}

		return 0;
	}
}
