#include "ScreenDevice.h"

#include "Logger/Logger.h"
#include "ResourceCore/ResourceInterface.h"

namespace
{
	sf::RenderWindow* window;
	Resource::ScreenResourceContent screenResource;
}

const Resource::ScreenResourceContent Device::DefaultScreenResource =
{
	// sf::VideoMode::getDesktopMode(),
	sf::VideoMode{ 400, 400 },
	sf::ContextSettings{ 4, 4, 8 }, true,
	Resource::ScreenResourceContent::VSync, 0
};

namespace Device
{
	bool Screen::Initialize()
	{
		// window must be initiated dynamically because
		// of sfml internals
		window = new sf::RenderWindow();

		if (Resource::ScreenResource resource; Resource::Interface::LoadResource(
				L"screen",
				&resource,
				Resource::SettingsResourceDefinition))
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

	Resource::ScreenResourceContent& Screen::ChangeScreenResource()
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
		case Resource::ScreenResourceContent::Limit::Framelimit:
			window->setFramerateLimit(screenResource.limit);
			window->setVerticalSyncEnabled(false);

			break;
		case Resource::ScreenResourceContent::Limit::None:
			window->setFramerateLimit(0);
			window->setVerticalSyncEnabled(false);

			break;
		case Resource::ScreenResourceContent::Limit::VSync:
			window->setFramerateLimit(0);
			window->setVerticalSyncEnabled(true);

			break;
		}
	}

	bool Screen::SaveScreenResource()
	{
		Resource::ScreenResource resource;
		resource.adopt(screenResource);
		return Resource::Interface::SaveResource(
			L"screen",
			&resource,
			Resource::SettingsResourceDefinition);
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
