#pragma once

#include <Client/source/menu/element/LogicScrollBar.h>

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

namespace Menu
{
	class SimpleScrollBar
		:
		public LogicScrollBar
	{
	public:
		struct Style
		{
			float padding;

			sf::Vector2f
				size,
				position;

			sf::Color
				background_fillColor,
				background_outlineColor,

				default_fillColor,
				default_outlineColor,

				enter_fillColor,
				enter_outlineColor,

				click_fillColor,
				click_outlineColor;

			float
				background_outlineThickness,
				default_outlineThickness,
				enter_outlineThickness,
				click_outlineThickness;
		};

		SimpleScrollBar(
			ElementBase* const parent,
			const Style style,
			const Direction direction,
			const sf::View* const view)
			:
			style(style),
			LogicScrollBar(
				parent,
				direction,
				view)
		{
			useOnLogic = false;

			loadPosition();
			setupStyle();
		}

		virtual void onDraw() override;

		void reset();

		void setConsumption(
			const float percent);
		void setPosition(
			const float position);
		
		// Position of the scrollbar as
		float getCurrentPosition() const
		{
			return currentPosition;
		}

		sf::Vector2f getPosition() const override
		{
			return shape.getPosition();
		}

		sf::Vector2f getSize() const override
		{
			return shape.getSize();
		}

		void loadPosition() override
		{
			shape.setPosition(convertPosition(
				style.position
			));
			
			shape.setSize(
				style.size
			);

			setConsumption(1.f);
		}

	protected:
		virtual void onScrollBarMoved() override;
		virtual void onMouseEnter() override;
		virtual void onMouseLeave() override;
		
		virtual void onMouseClickBegin() override;
		virtual void onMouseClickEnd() override;

	private:
		const Style style;

		void updatePosition();

		void setupStyle();
		void setEnterStyle();
		void setDefaultStyle();

		bool checkIsInside(
			const int x,
			const int y) const override;

		sf::RectangleShape shape, scrollBar;
	};


	inline void SimpleScrollBar::onDraw()
	{
		DEVICE::Interface::getScreen()->onDraw(&shape);
		DEVICE::Interface::getScreen()->onDraw(&scrollBar);
	}

	inline void SimpleScrollBar::reset()
	{
		setPosition(0.f);
	}

	inline void SimpleScrollBar::setPosition(
		const float position)
	{
		if (currentPosition != position)
		{
			currentPosition = position;
		}

		onScrollBarMoved();
	}

	inline void SimpleScrollBar::onScrollBarMoved()
	{
		updatePosition();
	}

	inline void SimpleScrollBar::onMouseEnter()
	{
		setEnterStyle();
	}

	inline void SimpleScrollBar::onMouseLeave()
	{
		setDefaultStyle();
	}

	inline void SimpleScrollBar::onMouseClickEnd()
	{
		if (getIsInside())
		{
			setEnterStyle();
		}
		else
		{
			setDefaultStyle();
		}
	}

	inline void SimpleScrollBar::updatePosition()
	{
		if (getDirection() == Direction::Horizontal)
		{
			scrollBar.setPosition(convertPosition(
				style.position.x + style.padding + currentPosition,
				scrollBar.getPosition().y
			));
		}
		else
		{
			scrollBar.setPosition(convertPosition(
				scrollBar.getPosition().x,
				style.position.y + style.padding + currentPosition
			));
		}
	}
}
