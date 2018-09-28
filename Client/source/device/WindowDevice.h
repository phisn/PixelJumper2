#pragma once

#include "Settings.h"

#include <SFML/Graphics.hpp>

#ifndef DEVICE
#define DEVICE ::Device
#endif

namespace Device
{
	namespace Window
	{
		bool initialize();

		void hide();
		void show();

		void display();
		void clear();

		void draw(sf::Drawable& drawble)
		{
			window.draw(drawble);
		}

		bool pollEvent(sf::Event* event);

		bool loadVideoSettings();
		bool saveVideoSettings();

		Settings::Video* getVideoSettingsP()
		{
			return &video;
		}
	}
}
