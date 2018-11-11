#include "ButtonBase.h"

namespace Menu
{
	void ButtonBase::setEnterStyle()
	{
		shape.setFillColor(
			style.enter_fillColor);
		shape.setOutlineColor(
			style.enter_outlineColor);
		shape.setOutlineThickness(
			style.enter_outlineThickness);
	}
	
	void ButtonBase::setDefaultStyle()
	{
		shape.setFillColor(
			style.default_fillColor);
		shape.setOutlineColor(
			style.default_outlineColor);
		shape.setOutlineThickness(
			style.default_outlineThickness);
	}

	void ButtonBase::setClickStyle()
	{
		shape.setFillColor(
			style.click_fillColor);
		shape.setOutlineColor(
			style.click_outlineColor);
		shape.setOutlineThickness(
			style.click_outlineThickness);
	}
}
