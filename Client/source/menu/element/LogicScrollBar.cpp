#include "LogicScrollBar.h"

namespace Menu
{
	void LogicScrollBar::onEvent(
		const sf::Event event)
	{
		switch (event.type)
		{
		case sf::Event::MouseButtonPressed:
			beginInside = isInside;

			if (isInside)
			{
				handleInsidePressed();
			}

			break;
		case sf::Event::MouseButtonReleased:
			if (beginInside)
			{
				beginInside = false;

				onMouseClickEnd();
			}

			break;
		case sf::Event::MouseMoved:
			if (beginInside)
			{
				handleMoveScrollBar(event);
			}
			else
			{
				handleInsideMove(event);
			}

			break;
		}
	}
	
	void LogicScrollBar::handleInsidePressed(
		const sf::Event event)
	{
		const sf::Vector2f point = DEVICE::Interface::getScreen()->getWindow()->mapPixelToCoords(
			sf::Vector2i(event.mouseButton.x, event.mouseButton.y)
		);

		beginPosition = direction == Direction::Horizontal
			? point.x
			: point.y;

		onMouseClickBegin();
	}
	
	void LogicScrollBar::handleInsideMove(
		const sf::Event event)
	{
		const bool currentIsInside = checkIsInside(
			event.mouseMove.x,
			event.mouseMove.y);

		if (currentIsInside != isInside)
		{
			isInside = currentIsInside;

			if (isInside)
			{
				onMouseEnter();
			}
			else
			{
				onMouseLeave();
			}
		}
	}
	
	void LogicScrollBar::handleMoveScrollBar(
		const sf::Event event)
	{
		const sf::Vector2f point = DEVICE::Interface::getScreen()->getWindow()->mapPixelToCoords(
			sf::Vector2i(event.mouseMove.x, event.mouseMove.y)
		);

		const float position = direction == Direction::Horizontal
			? point.x
			: point.y;

		currentPosition = position - beginPosition;

		if (currentPosition < 0)
		{
			currentPosition = 0;
		}
		else
			if (currentPosition > length)
			{
				currentPosition = length;
			}

		onScrollBarMoved();
	}
}
