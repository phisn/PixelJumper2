#include "WindowDevice.h"

namespace Device
{
	namespace
	{
		Settings::Video video;
		sf::RenderWindow window;
	}

	bool Window::initialize()
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

	bool Window::saveVideoSettings()
	{
		return true;
	}

	bool Window::loadVideoSettings()
	{
		return true;
	}



	void Window::hide()
	{
		window.close();
	}

	void Window::show()
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

	void Window::display()
	{
		window.display();
	}

	void Window::clear()
	{
		window.clear();
	}

	bool Window::pollEvent(
		sf::Event* event)
	{
		return window.pollEvent(*event);
	}
}
