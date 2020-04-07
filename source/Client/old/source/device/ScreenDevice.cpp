#include "ScreenDevice.h"

#include <Client/source/logger/Logger.h>
#include <Client/source/device/DeviceConfig.h>

namespace
{
	sf::RenderWindow* window;
	Device::ScreenSettings* resource;

	sf::Cursor defaultCursor;
	sf::Cursor textCursor;
}

namespace Device
{
	bool Screen::Initialize()
	{
		window = new sf::RenderWindow();
		resource = new Device::ScreenSettings();

		return Cursor::Initialize() && DefaultSettings();
	}

	void Screen::Uninitialize()
	{
		window->close();
		
		delete window;
		delete resource;
	}

	bool Screen::RecreateWindow()
	{
		_CloseWindow();
		_CreateWindow();

		return true;
	}

	bool Screen::DefaultSettings()
	{
		if (window->isOpen())
		{
			_CloseWindow();
		}

		// ... load settings
		resource->Content.antialiasing = 4;
		resource->Content.customVideoMode = sf::VideoMode(900, 900);
		resource->Content.framelimit = 60;
		resource->Content.hideCursor = true;
		resource->Content.pauseOnLeave = true;
		resource->Content.useCustomCursor = false;
		resource->Content.useDesktopVideoMode = false;
		resource->Content.useFramelimit = false;
		resource->Content.useFullscreen = false;
		resource->Content.useVSync = true;

		_CreateWindow();

		return true;
	}

	void Screen::_CloseWindow()
	{
		window->close();
	}

	void Screen::_CreateWindow()
	{
		sf::VideoMode videoMode;

		if (resource->Content.useDesktopVideoMode)
		{
			videoMode = sf::VideoMode::getDesktopMode();
		}
		else
		{
			videoMode = resource->Content.customVideoMode;
		}

		sf::Uint32 style = 0;

		if (resource->Content.useFullscreen)
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
			sf::ContextSettings(0U, 0U, resource->Content.antialiasing));
		
		if (resource->Content.useFramelimit)
		{
			window->setFramerateLimit(resource->Content.framelimit);
		}

		window->setVerticalSyncEnabled(resource->Content.useVSync);
	}

	bool Screen::Cursor::Initialize()
	{
		if (!defaultCursor.loadFromSystem(sf::Cursor::Type::Arrow))
		{
			Log::Error(L"Failed to load arrow cursor");
			return false;
		}

		if (!textCursor.loadFromSystem(sf::Cursor::Type::Text))
		{
			Log::Error(L"Failed to load text cursor");
			return false;
		}

		return true;
	}

	void Screen::Cursor::SetCursor(const sf::Cursor::Type cursor)
	{
		switch (cursor)
		{
		case sf::Cursor::Type::Arrow:
			window->setMouseCursor(defaultCursor);

			break;
		case sf::Cursor::Type::Text:
			window->setMouseCursor(textCursor);

			break;
		}
	}

	bool Screen::LoadSettings()
	{
		return Config::LoadType(
			resource, 
			ConfigType::Screen);
	}

	ScreenSettings* Screen::ChangeSettings()
	{
		return resource;
	}

	bool Screen::SaveSettings()
	{
		return Config::SaveType(
			resource, 
			ConfigType::Screen);
	}

	const sf::RenderWindow* Screen::GetWindow()
	{
		return window;
	}

	sf::RenderTarget* Screen::GetTarget()
	{
		return window;
	}

	void Screen::ResetView()
	{
		window->setView(window->getDefaultView());
	}

	void Screen::SetView(
		const sf::View& view)
	{
		window->setView(view);
	}

	const sf::View& Screen::GetView()
	{
		return window->getView();
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
