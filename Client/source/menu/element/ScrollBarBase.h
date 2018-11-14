#pragma once

#include <Client/source/menu/Common.h>
#include <Client/source/menu/element/ElementBase.h>

namespace Menu
{
	class ScrollBarBase
		:
		public ElementBase
	{
	public:
		struct Style
		{
			sf::Color
				default_scrollBar_color,
				default_background_color,

				enter_scrollBar_color,
				enter_background_color,

				click_scrollBar_color,
				click_background_color;
		};

		ScrollBarBase(
			const Style style,
			const Direction direction)
			:
			style(style),
			direction(direction)
		{
			useOnEvent = true;
		}

		virtual ~ScrollBarBase() { }

		Direction getDirection() const
		{
			return direction;
		}

		virtual void onEvent(
			const sf::Event event) override;
	protected:
		void setEnterStyle() 
		{
			background.setFillColor(style.enter_background_color);
			scrollBar.setFillColor(style.enter_scrollBar_color);
		}

		void setDefaultStyle()
		{
			background.setFillColor(style.default_background_color);
			scrollBar.setFillColor(style.default_scrollBar_color);
		}

		void setClickStyle()
		{
			background.setFillColor(style.click_background_color);
			scrollBar.setFillColor(style.click_scrollBar_color);
		}

		bool getIsInside() const
		{
			return isInside;
		}

		float
			length,
			currentPosition;

		virtual void onScrollBarMoved() = 0;

		virtual void onMouseEnter() { setEnterStyle(); }
		virtual void onMouseLeave() { setDefaultStyle(); }

		virtual void onMouseClickBegin() { setClickStyle(); }
		virtual void onMouseClickEnd()
		{
			isInside ? setEnterStyle() : setDefaultStyle();
		}

		bool checkIsInside(const sf::Vector2f p) const
		{
			return scrollBar.getPosition().x < p.x
				&& scrollBar.getPosition().y < p.y
				&& scrollBar.getPosition().x + scrollBar.getSize().x > p.x
				&& scrollBar.getPosition().y + scrollBar.getSize().y > p.y;
		}
	private:
		const Style style;
		const Direction direction;

		float beginPosition;
		bool
			beginInside = false,
			isInside = false;

		sf::RectangleShape background, scrollBar;
	};
}
