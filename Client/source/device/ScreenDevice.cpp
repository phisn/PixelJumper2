#include "WindowDevice.h"
#include "ScreenDevice.h"

namespace Device
{
	namespace
	{
		Settings::Video video;
		sf::RenderWindow window;
	}

	bool Screen::initialize()
	{
		if (!loadVideoSettings())
		{
			saveVideoSettings();

			if (!loadVideoSettings())
			{
				return false;
			}
		}

		// ...

		return true;
	}

	bool Screen::saveVideoSettings()
	{
		return true;
	}

	bool Screen::loadVideoSettings()
	{
		return true;
	}

	void Screen::hide()
	{
		window.close();
	}

	void Screen::show()
	{
		window.create(
			video.videMode,
			L"",
			video.getStyle(),
			video.settings);

		window.setVerticalSyncEnabled(
			video.vsyncEneabled);
		window.setFramerateLimit(
			video.frameLimit);
	}

	sf::RenderWindow& Screen::getWindow()
	{
		return window;
	}

	Settings::Video* Screen::getVideoSettingsP()
	{
		return &video;
	}
}
