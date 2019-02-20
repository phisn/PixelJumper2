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
		CloseWindow();
		CreateWindow();
	}

	bool Screen::ReloadSettings()
	{

		RecreateWindow();
	}

	bool Screen::ApplySettings(
		ScreenResource* const resource)
	{
	}

	void Screen::CloseWindow()
	{
	}

	void Screen::CloseWindow()
	{
		window->close();
	}

	void Screen::CreateWindow()
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
}
