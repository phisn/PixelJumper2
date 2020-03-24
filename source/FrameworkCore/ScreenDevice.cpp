#include "ScreenDevice.h"

#include "Logger/Logger.h"
#include "ResourceCore/ResourceInterface.h"

namespace
{
	sf::RenderWindow* window;
	Device::ScreenResourceContent screenResource;
}

const Device::ScreenResourceContent Device::DefaultScreenResource =
{
	sf::VideoMode::getDesktopMode(),
	sf::ContextSettings{ }, false,
	Device::ScreenResourceContent::VSync, 0
};

namespace Device
{
	bool Screen::Initialize()
	{
		// window must be initiated dynamically because
		// of sfml internals
		window = new sf::RenderWindow();

		if (ScreenResource resource; Resource::Interface::LoadResource(
				&resource,
				Resource::ResourceType::Settings,
				L"screen"))
		{
			screenResource = resource.content;
		}
		else
		{
			Log::Warning(L"screen resource not found. creating new one");

			ResetScreenResource();
			if (!SaveScreenResource())
				Log::Error(L"failed to save new screen resource");
		}

		LoadScreenResource();

		return true;
	}

	void Screen::Uninitialize()
	{
		window->close();
		delete window;
	}

	ScreenResourceContent& Screen::ChangeScreenResource()
	{
		return screenResource;
	}

	void Screen::LoadScreenResource()
	{
		// does automatically close old window
		window->create(
			screenResource.videomode, 
			"Pixel Jumper 2", 
			(screenResource.windowed
				? 7U 
				: sf::Style::Fullscreen), 
			screenResource.settings);

		switch (screenResource.limit)
		{
		case ScreenResourceContent::Limit::Framelimit:
			window->setFramerateLimit(screenResource.limit);
			window->setVerticalSyncEnabled(false);

			break;
		case ScreenResourceContent::Limit::None:
			window->setFramerateLimit(0);
			window->setVerticalSyncEnabled(false);

			break;
		case ScreenResourceContent::Limit::VSync:
			window->setFramerateLimit(0);
			window->setVerticalSyncEnabled(true);

			break;
		}
	}

	bool Screen::SaveScreenResource()
	{
		ScreenResource resource;
		resource.adopt(screenResource);
		return Resource::Interface::SaveResource(
			&resource,
			Resource::ResourceType::Settings,
			L"screen");
	}

	void Screen::ResetScreenResource()
	{
		screenResource = DefaultScreenResource;
	}

	sf::RenderWindow* Screen::GetWindow()
	{
		return window;
	}
}
