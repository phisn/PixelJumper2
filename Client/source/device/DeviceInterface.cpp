#include <Client/source/device/InputDevice.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/scene/SceneInterface.h>

#include "DeviceInterface.h"

namespace
{
	Device::GlobalInput* input;
	Device::Screen* screen;
}

namespace Device
{
	bool Interface::initialize()
	{
		input = new GlobalInput();
		screen = new Screen();

		if (!screen->initalize())
		{
			return false;
		}

		if (!Scene::Interface::pushScene(
				new ))
		{

		}

		return true;
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
}
