#include "BootScene.h"

namespace Scene
{
	bool Boot::onCreate()
	{
		return true;
	}

	bool Boot::onRemove()
	{
		return true;
	}

	void Boot::onShow()
	{
		// Work on arguments
		
		if (!Scene::Handler::pushScene(
			new Scene::DebugMenu()))
		{
			MessageBox(
				NULL,
				L"Failed to start debug menu",
				L"Error",
				MB_OK);
		}
	}

	void Boot::onHide()
	{
	}

	void Boot::onEvent(
		sf::Event event)
	{
	}

	void Boot::onLogic(
		sf::Time time)
	{
	}

	void Boot::onDraw(
		::Device::Window * window)
	{
	}
}
