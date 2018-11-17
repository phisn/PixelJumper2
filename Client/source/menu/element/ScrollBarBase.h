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

		void setup(
			ElementBase::Properties* const properties) override;

		virtual void onEvent(
			const sf::Event event) override;
		virtual void onDraw() override;

		virtual void resetPosition() override;
		virtual void resetSize() override;

		void setConsumption(
			const float consumption);

		float getOffset() const
		{
			return scrollBarOffset + scrollBarPosition;
		}

	protected:
		void updatePosition();
		void updateConsumption();

		void setEnterStyle();
		void setDefaultStyle();
		void setClickStyle();

		bool getIsInside() const;
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

		bool checkIsInside(const sf::Vector2f p) const;
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

	inline void ScrollBarBase::onDraw()
	{
		DEVICE::Interface::getScreen()->onDraw(&background);
		DEVICE::Interface::getScreen()->onDraw(&scrollBar);
	}

	inline void ScrollBarBase::resetSize()
	{
		ElementBase::resetSize();

		background.setSize(getSize());
		updateConsumption();
	}

	inline void ScrollBarBase::setConsumption(
		const float consumption)
	{
		this->consumption = consumption;

		updateConsumption();
	}

	inline bool ScrollBarBase::getIsInside() const
	{
		return isInside;
	}

	inline bool ScrollBarBase::checkIsInside(
		const sf::Vector2f p) const
	{
		return scrollBar.getPosition().x < p.x
			&& scrollBar.getPosition().y < p.y
			&& scrollBar.getPosition().x + scrollBar.getSize().x > p.x
			&& scrollBar.getPosition().y + scrollBar.getSize().y > p.y;
	}
}
