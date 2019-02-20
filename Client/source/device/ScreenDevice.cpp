#include "ScreenDevice.h"

#include <Client/source/logger/Logger.h>

namespace
{
	sf::RenderWindow* window;
	Device::ScreenResource* resource;
}

namespace Device
{
	bool Screen::Initialize()
	{
		window = new sf::RenderWindow();
		resource = new Device::ScreenResource();

		return ReloadSettings();
	}

	void Screen::Uninitialize()
	{
		delete window;
		delete resource;
	}

	bool Screen::RecreateWindow()
	{
		_CloseWindow();
		_CreateWindow();
	}

	bool Screen::ReloadSettings()
	{
		if (window->isOpen())
		{
			_CloseWindow();
		}

		// ... load settings
		resource->Settings.antialiasing = 4;
		resource->Settings.customVideoMode = sf::VideoMode(600, 400);
		resource->Settings.framelimit = 60;
		resource->Settings.hideCursor = true;
		resource->Settings.pauseOnLeave = true;
		resource->Settings.useCustomCursor = false;
		resource->Settings.useDesktopVideoMode = false;
		resource->Settings.useFramelimit = true;
		resource->Settings.useFullscreen = false;
		resource->Settings.useVSync = true;

		_CreateWindow();
	}

	void Screen::_CloseWindow()
	{
	}

	void Screen::_CloseWindow()
	{
		window->close();
	}

	void Screen::_CreateWindow()
	{
		sf::VideoMode videoMode;

		if (resource->Settings.useDesktopVideoMode)
		{
			videoMode = sf::VideoMode::getDesktopMode();
		}
		else
		{
			videoMode = resource->Settings.customVideoMode;
		}

		sf::Uint32 style = 0;

		if (resource->Settings.useFullscreen)
		{
			style = sf::Style::Fullscreen;
		}
		else
		{
			style = sf::Style::Close | sf::Style::Titlebar;
		}

		window->create(
			videoMode,
			"Pixel Jumper 2",
			style,
			sf::ContextSettings(0U, 0U, resource->Settings.antialiasing));

		if (resource->Settings.useFramelimit)
		{
			window->setFramerateLimit(resource->Settings.framelimit);
		}

		window->setVerticalSyncEnabled(resource->Settings.useVSync);
	}

	const sf::RenderWindow* Screen::GetWindow()
	{
		return window;
	}

	void Screen::BeginDraw()
	{
		window->clear();
	}

	void Screen::EndDraw()
	{
		window->display();
	}

	void Screen::Draw(
		const sf::Drawable& drawable)
	{
		window->draw(drawable);
	}

	bool Screen::PollEvent(sf::Event& event)
	{
		return window->pollEvent(event);
	}
}
