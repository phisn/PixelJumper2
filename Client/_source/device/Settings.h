#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <string>

namespace Device
{
	namespace Settings
	{
		struct Video
		{
			sf::Uint32 getStyle() const
			{
				return isFullscreen
					? sf::Style::Fullscreen
					: sf::Style::Close |
					  sf::Style::Titlebar;
			}

			sf::VideoMode videMode = sf::VideoMode::getDesktopMode();
			sf::ContextSettings settings;

			bool isFullscreen = true;

			bool vsyncEneabled = true;
			int frameLimit = 60;
		};

		struct Audio
		{
		};
	}
}
