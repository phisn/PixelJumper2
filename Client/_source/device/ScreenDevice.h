#pragma once

#include "Settings.h"

#include <SFML/Graphics.hpp>

#ifndef DEVICE
#define DEVICE ::Device
#endif

namespace Device
{
	namespace Screen
	{
		bool initialize();

		void hide();
		void show();

		sf::RenderWindow* getWindow();

		bool loadVideoSettings();
		bool saveVideoSettings();

		Settings::Video* getVideoSettingsP();
	}
}
