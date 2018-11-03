#include "LogicMenuButton.h"

namespace Menu
{
	void LogicButton::onEvent(
		const sf::Event event)
	{
		switch (event.type)
		{
		case sf::Event::MouseButtonPressed:
			beginInside = isInside;

			if (isInside)
			{
				onMouseClickBegin();
			}

			break;
		case sf::Event::MouseButtonReleased:
			if (beginInside)
			{
				onMouseClickEnd();

				if (isInside)
				{
					onMouseClick();
				}
			}

			break;
		case sf::Event::MouseMoved:
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

			break;
		}
	}
}
