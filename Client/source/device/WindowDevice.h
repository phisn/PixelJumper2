#pragma once

#include <SFML/Graphics.hpp>

namespace Device
{
	class Window
	{
	public:
		bool initialize();

		void display();
		void clear();
		bool pollEvent(sf::Event* event);
	private:
		sf::RenderWindow window;
	};

	inline void Window::display()
	{
		window.display();
	}

	inline void Window::clear()
	{
		window.clear();
	}

	inline bool Window::pollEvent(sf::Event* event)
	{
		return window.pollEvent(*event);
	}
}
