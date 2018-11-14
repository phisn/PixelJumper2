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
		struct Properties
			:
			public ElementBase::Properties
		{
			float padding;
		};

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

			setDefaultStyle();
		}

		virtual ~ScrollBarBase() { }

		void initialize(
			ElementBase::Properties* const properties) override
		{
			ElementBase::initialize(properties);

			padding = ((Properties*) properties)->padding;
		}

		virtual void onEvent(
			const sf::Event event) override;
		virtual void onDraw() override
		{
			DEVICE::Interface::getScreen()->onDraw(&background);
			DEVICE::Interface::getScreen()->onDraw(&scrollBar);
		}

		virtual void resetPosition() override
		{
			ElementBase::resetPosition();

			background.setPosition( getPosition() );
			background.setSize( getSize() );

			updateConsumption();
		}

		void setConsumption(
			const float consumption)
		{
			this->consumption = consumption;

			updateConsumption();
		}

	protected:
		void updatePosition()
		{
			float x_position = getPosition().x + padding;
			float y_position = getPosition().y + padding;

			(	direction == Direction::Horizontal
					? x_position
					: y_position
			) += scrollBarOffset + scrollBarPosition;

			scrollBar.setPosition(
				x_position,
				y_position);
		}

		void updateConsumption()
		{
			scrollBarOffset = 0.f;
			updatePosition();

			float x_size = getSize().x - padding * 2;
			float y_size = getSize().y - padding * 2;

			(	direction == Direction::Horizontal 
					? x_size 
					: y_size
			) *= consumption;

			length = direction == Direction::Horizontal
				? getSize().x - x_size
				: getSize().y - y_size;

			scrollBar.setSize(sf::Vector2f(
				x_size, y_size
			));
		}

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
			scrollBarPosition,
			scrollBarOffset;

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

		float
			padding,
			beginMousePosition,
			consumption = 1.f;
		bool
			beginInside = false,
			isInside = false;

		sf::RectangleShape background, scrollBar;
	};
}
