#include "ScreenDevice.h"

#include <Client/source/logger/Logger.h>

bool Device::Screen::initalize()
{
	Log::Section section(L"Initializing ScreenDevice");

	// Load screen settings from file

	settings = defaultScreenSettings;

	return true;
}

void Device::Screen::onShow()
{
	window.create(
		settings.videoMode, 
		L"Pixel Jumper 2", 
		settings.style, 
		settings.settings);

	window.setFramerateLimit(60);
}

void Device::Screen::onHide()
{
	window.close();
}
