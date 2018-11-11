#include "SimpleScrollBar.h"

namespace Menu
{
	void SimpleScrollBar::setConsumption(
		const float percent)
	{
		reset();

		if (getDirection() == Direction::Horizontal)
		{
			scrollBar.setSize(sf::Vector2f(
			   (properties.size.x - properties.padding * 2) * percent,
				properties.size.y - properties.padding * 2
			));

			length = scrollBar.getSize().x;
		}
		else
		{
			scrollBar.setSize(sf::Vector2f(
				properties.size.x - properties.padding * 2,
				(properties.size.y - properties.padding * 2) * percent
			));

			length = scrollBar.getSize().y;
		}
	}

	void SimpleScrollBar::onMouseClickBegin()
	{
		scrollBar.setFillColor(
			style.click_fillColor);
		scrollBar.setOutlineColor(
			style.click_outlineColor);
		scrollBar.setOutlineThickness(
			style.click_outlineThickness);
	}

	void SimpleScrollBar::setupStyle()
	{
		shape.setFillColor(
			style.background_fillColor);
		shape.setOutlineColor(
			style.background_outlineColor);
		shape.setOutlineThickness(
			style.background_outlineThickness);

		setDefaultStyle();
	}

	void SimpleScrollBar::setEnterStyle()
	{
		scrollBar.setFillColor(
			style.enter_fillColor);
		scrollBar.setOutlineColor(
			style.enter_outlineColor);
		scrollBar.setOutlineThickness(
			style.enter_outlineThickness);
	}

	void SimpleScrollBar::setDefaultStyle()
	{
		scrollBar.setFillColor(
			style.default_fillColor);
		scrollBar.setOutlineColor(
			style.default_outlineColor);
		scrollBar.setOutlineThickness(
			style.default_outlineThickness);
	}

	bool SimpleScrollBar::checkIsInside(
		const int x, 
		const int y) const
	{
		sf::Vector2f pos = DEVICE::Interface::getScreen()->getWindow()->mapPixelToCoords(sf::Vector2i(x, y));

		return
			shape.getPosition().x >= pos.x &&
			shape.getPosition().y >= pos.y &&
			shape.getPosition().x + shape.getSize().x <= pos.x &&
			shape.getPosition().y + shape.getSize().y <= pos.y;
	}
}
