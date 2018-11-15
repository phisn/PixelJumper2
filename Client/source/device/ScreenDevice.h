#pragma once

#include <SFML/Graphics.hpp>

namespace Device
{
	const sf::Int32 fullscreen = sf::Style::Fullscreen;
	const sf::Int32 windowed = sf::Style::Close | sf::Style::Titlebar;

	const struct ScreenSettings
	{
		sf::Int32 style;

		sf::VideoMode videoMode;
		sf::ContextSettings settings;
	} defaultScreenSettings =
	{
		fullscreen,
		sf::VideoMode::getDesktopMode(),
		sf::ContextSettings::ContextSettings(0U, 0U, 0U) // For AA
	};

	class Screen
	{
	public:
		bool initalize();

		void onShow();
		void onHide();

		void onDraw(
			const sf::Drawable* drawable)
		{
			window.draw(*drawable);
		}

		void applyView(
			const sf::View* view)
		{
			window.setView(*view);
		}

		void resetView()
		{
			window.setView(
				window.getDefaultView()
			);
		}

		sf::RenderWindow* getWindow()
		{
			return &window;
		}
	private:
		sf::RenderWindow window;

		ScreenSettings settings;
	};
}
