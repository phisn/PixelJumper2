#pragma once

#include <Client/source/menu/element/LogicButton.h>

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

namespace Menu
{
	class SimpleButton
		:
		public LogicButton
	{
	public:
		struct Style
		{
			sf::Vector2f
				size,
				position;

			sf::Color
				default_fillColor,
				default_outlineColor,

				enter_fillColor,
				enter_outlineColor,

				click_fillColor,
				click_outlineColor;

			float
				default_outlineThickness,
				enter_outlineThickness,
				click_outlineThickness;
		};

		SimpleButton(
			const Style style)
			:
			style(style)
		{
			shape.setSize(
				style.size);
			shape.setPosition(
				style.position);

			setDefaultStyle();
		}

		virtual ~SimpleButton() {  }

		virtual void onDraw() override
		{
			DEVICE::Interface::getScreen()->onDraw(&shape);
		}

		sf::Vector2f getPosition() const override
		{
			return shape.getPosition();
		}

		sf::Vector2f getSize() const override
		{
			return shape.getSize();
		}

	protected:
		virtual void onMouseEnter() override;
		virtual void onMouseLeave() override;

		virtual void onMouseClickBegin() override;
		virtual void onMouseClickEnd() override;

	private:
		const Style style;

		void setEnterStyle();
		void setDefaultStyle();

		bool checkIsInside(
			const int x,
			const int y) const override;

		sf::RectangleShape shape;
	};

	inline void SimpleButton::onMouseEnter()
	{
		setEnterStyle();
	}

	inline void SimpleButton::onMouseLeave()
	{
		setDefaultStyle();
	}

	inline void SimpleButton::onMouseClickEnd()
	{
		setEnterStyle();
	}
}
