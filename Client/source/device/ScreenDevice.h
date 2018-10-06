#pragma once

#include <SFML/Graphics.hpp>

namespace Device
{
	class Screen
	{
	public:
		~Screen()
		{
			if (window)
			{
				delete window;
			}
		}

		bool initalize();

		void onShow();
		void onHide();

		void draw(
			const sf::Drawable* drawable)
		{
			window->draw(*drawable);
		}

		void applyView(
			const sf::View* view)
		{
			window->setView(*view);
		}

		const sf::RenderWindow* getWindow() const
		{
			return window;
		}
	private:
		sf::RenderWindow* window;
	};
}
