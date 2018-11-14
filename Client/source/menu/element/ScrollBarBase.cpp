#include "ScrollBarBase.h"

namespace Menu
{
	void ScrollBarBase::onEvent(
		const sf::Event event)
	{
		switch (event.type)
		{
		case sf::Event::MouseButtonPressed:
			if (isInside)
			{
				beginInside = isInside;

				const sf::Vector2f p = convertPixel(
					event.mouseMove.x,
					event.mouseMove.y);

				beginPosition = direction == Direction::Horizontal
					? p.x : p.y;

				onMouseClickEnd();
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
				const sf::Vector2f p = convertPixel(
					event.mouseMove.x,
					event.mouseMove.y);

				const float position = direction == Direction::Horizontal
					? p.x : p.y,
					oldCurrentPosition = currentPosition;

				currentPosition = position - beginPosition;

				if (currentPosition < 0)
				{
					currentPosition = 0;

					if (oldCurrentPosition == 0)
						break;
				}
				else
					if (currentPosition > length)
					{
						currentPosition = length;

						if (oldCurrentPosition == length)
							break;
					}

				onScrollBarMoved();
			}
			else
			{
				const bool temp = checkIsInside(convertPixel(
					event.mouseMove.x,
					event.mouseMove.y
				));

				if (temp != isInside)
				{
					isInside = temp;

					if (temp)
					{
						onMouseEnter();
					}
					else
					{
						onMouseLeave();
					}
				}
			}

			break;
		}
	}
}
