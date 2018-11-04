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
			   (style.size.x - style.padding * 2) * percent,
				style.size.y - style.padding * 2
			));

			length = scrollBar.getSize().x;
		}
		else
		{
			scrollBar.setSize(sf::Vector2f(
				style.size.y - style.padding * 2,
				(style.size.y - style.padding * 2) * percent
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
		return
			shape.getPosition().x >= x &&
			shape.getPosition().y >= y &&
			shape.getPosition().x + shape.getSize().x <= x &&
			shape.getPosition().y + shape.getSize().y <= y;
	}
}
