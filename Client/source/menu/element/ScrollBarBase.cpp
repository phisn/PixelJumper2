#include "ScrollBarBase.h"

namespace Menu
{
	void ScrollBarBase::initialize(
		ElementBase::Properties* const properties)
	{
		ElementBase::initialize(properties);

		padding = ((Properties*)properties)->padding;
	}

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
					scrollBarOffset = -scrollBarPosition;

					if (oldScrollBarOffset == scrollBarOffset)
						break;
				}
				else
					if (scrollBarOffset + scrollBarPosition > length)
					{
						scrollBarOffset = length - scrollBarPosition;

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

	void ScrollBarBase::resetPosition()
	{
		ElementBase::resetPosition();

		background.setPosition(getPosition());
		background.setSize(getSize());

		updateConsumption();
	}

	void ScrollBarBase::updatePosition()
	{
		float x_position = getPosition().x + padding;
		float y_position = getPosition().y + padding;

		(direction == Direction::Horizontal
			? x_position
			: y_position
			) += scrollBarOffset + scrollBarPosition;

		scrollBar.setPosition(
			x_position,
			y_position);
	}

	void ScrollBarBase::updateConsumption()
	{
		scrollBarOffset = 0.f;
		updatePosition();

		float x_size = getSize().x - padding * 2;
		float y_size = getSize().y - padding * 2;

		(direction == Direction::Horizontal
			? x_size
			: y_size
			) *= consumption;

		length = direction == Direction::Horizontal
			? getSize().x - x_size - padding * 2
			: getSize().y - y_size - padding * 2;

		scrollBar.setSize(sf::Vector2f(
			x_size, y_size
		));
	}

	void ScrollBarBase::setEnterStyle()
	{
		background.setFillColor(style.enter_background_color);
		scrollBar.setFillColor(style.enter_scrollBar_color);
	}

	void ScrollBarBase::setDefaultStyle()
	{
		background.setFillColor(style.default_background_color);
		scrollBar.setFillColor(style.default_scrollBar_color);
	}

	void ScrollBarBase::setClickStyle()
	{
		background.setFillColor(style.click_background_color);
		scrollBar.setFillColor(style.click_scrollBar_color);
	}
}
