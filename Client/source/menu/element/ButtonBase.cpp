#include "ButtonBase.h"
#include <iostream>
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

	void ButtonBase::onEvent(
		const sf::Event event)
	{
		switch (event.type)
		{
		case sf::Event::MouseButtonPressed:
			if (event.mouseButton.button == sf::Mouse::Left &&
				isInside)
			{
				beginInside = true;

				onMouseClickBegin();
			}

			break;
		case sf::Event::MouseButtonReleased:
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				if (beginInside)
				{
					beginInside = false;
					
					onMouseClickEnd();
					
					if (isInside)
					{
						onMouseClick();
					}
				}
			}

			break;
		case sf::Event::MouseMoved:
			const bool temp = checkIsInside(convertPixel(
				event.mouseMove.x,
				event.mouseMove.y));

			if (temp != isInside)
			{
				isInside = temp;

				if (beginInside)
				{
					break;
				}

				if (isInside)
				{
					onMouseEnter();
				}
				else
				{
					onMouseLeave();
				}
			}

			break;
		}
	}
	
	void ButtonBase::onDraw()
	{
		DEVICE::Interface::getScreen()->onDraw(&shape);
	}

	void ButtonBase::resetPosition()
	{
		ElementBase::resetPosition();

		shape.setPosition( getPosition() );
	}

	void ButtonBase::resetSize()
	{
		ElementBase::resetSize();

		shape.setSize( getSize() );
	}
}
