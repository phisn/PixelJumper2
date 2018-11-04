#include "SimpleButton.h"

namespace Menu
{
	void SimpleButton::onMouseClickBegin()
	{
		shape.setFillColor(
			style.click_fillColor);
		shape.setOutlineColor(
			style.click_outlineColor);
		shape.setOutlineThickness(
			style.click_outlineThickness);
	}

	void SimpleButton::setEnterStyle()
	{
		shape.setFillColor(
			style.enter_fillColor);
		shape.setOutlineColor(
			style.enter_outlineColor);
		shape.setOutlineThickness(
			style.enter_outlineThickness);
	}

	void SimpleButton::setDefaultStyle()
	{
		shape.setFillColor(
			style.default_fillColor);
		shape.setOutlineColor(
			style.default_outlineColor);
		shape.setOutlineThickness(
			style.default_outlineThickness);
	}

	bool SimpleButton::checkIsInside(
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
