#include "PixelJumperScene.h"

namespace Scene
{
	bool PixelJumper::onCreate()
	{
		shape.setFillColor(sf::Color::Red);
		shape.setPosition(10, 10);
		shape.setSize(sf::Vector2f(100, 100));

		return true;
	}

	bool PixelJumper::onRemove()
	{
		return true;
	}

	void PixelJumper::onShow()
	{
	}

	void PixelJumper::onHide()
	{
	}

	void PixelJumper::onEvent(
		sf::Event event)
	{
		switch (event.type)
		{
		case sf::Event::Resized:

		}
	}

	void PixelJumper::onLogic(
		sf::Time time)
	{
	}

	void PixelJumper::onDraw()
	{
		DEVICE::Window::draw(shape);
	}
}
