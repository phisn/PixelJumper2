#include "ScrollBarBase.h"

namespace Menu
{
	void ScrollBarBase::onEvent(
		const sf::Event event)
	{
		switch (event.type)
		{
		case sf::Event::MouseButtonPressed:
			if (event.mouseButton.button == sf::Mouse::Left && isInside)
			{
				beginInside = isInside;

				const sf::Vector2f p = convertPixel(
					event.mouseButton.x,
					event.mouseButton.y);

				beginMousePosition = direction == Direction::Horizontal
					? p.x : p.y;

				onMouseClickBegin();
			}

			break;
		case sf::Event::MouseButtonReleased:
			if (beginInside && event.mouseButton.button == sf::Mouse::Left)
			{
				scrollBarPosition += scrollBarOffset;
				scrollBarOffset = 0;

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

				const float mousePosition = direction == Direction::Horizontal
					? p.x : p.y,
					oldScrollBarOffset = scrollBarOffset;

				scrollBarOffset = mousePosition - beginMousePosition;

				if (scrollBarOffset + scrollBarPosition < 0)
				{
					scrollBarOffset = -(2 * scrollBarPosition + scrollBarOffset);

					if (oldScrollBarOffset == scrollBarOffset)
						break;
				}
				else
					if (scrollBarOffset + scrollBarPosition > length)
					{
						scrollBarOffset = (scrollBarOffset + scrollBarPosition) - length;

						if (oldScrollBarOffset == scrollBarOffset)
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
