#include "ScreenDevice.h"

bool Device::Screen::initalize()
{
	// Load screen settings from file

	settings = defaultScreenSettings;

	return true;
}

void Device::Screen::onShow()
{
	window->create(
		settings.videoMode, 
		L"Pixel Jumper 2", 
		settings.style, 
		settings.settings);
}

void Device::Screen::onHide()
{
	window->close();
}
