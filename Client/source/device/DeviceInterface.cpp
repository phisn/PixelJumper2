#include <Client/source/device/ScreenDevice.h>
#include <Client/source/device/InputDevice.h>
#include <Client/source/device/ResourceDevice.h>

#include <Client/source/scene/SceneInterface.h>
#include <Client/source/scene/DebugScene.h>

#include "DeviceInterface.h"

namespace
{
	Device::GlobalInput* input = NULL;
	Device::Screen* screen = NULL;
	Device::Resource* resource = NULL;
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

		if (!Scene::Interface::pushScene(
				new Scene::Debug())
			)
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
		return 0;
	}

	GlobalInput* Interface::getInput()
	{
		return input;
	}

	void Interface::onSceneTypeChanged(
		SCENE::Type oldType, 
		SCENE::Type newType)
	{
		if (oldType == SCENE::Type::SFML)
		{
			screen->onHide();

			return;
		}

		if (newType == SCENE::Type::SFML)
		{
			screen->onShow();
		}
	}

	Screen* Interface::getScreen()
	{
		return screen;
	}
	Resource * Interface::getResource()
	{
		return nullptr;
	}
}
