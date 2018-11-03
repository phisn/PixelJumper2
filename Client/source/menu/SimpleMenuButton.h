#pragma once

#include <Client/source/menu/LogicMenuButton.h>

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
			onMouseEnter();
		}

		virtual ~SimpleButton() {  }

		virtual void onDraw() override
		{
			DEVICE::Interface::getScreen()->onDraw(&shape);
		}
	protected:
		virtual void onMouseEnter()
		{
			shape.setFillColor(
				style.enter_fillColor);
			shape.setOutlineColor(
				style.enter_outlineColor);
			shape.setOutlineThickness(
				style.enter_outlineThickness);
		}

		virtual void onMouseLeave()
		{
			shape.setFillColor(
				style.default_fillColor);
			shape.setOutlineColor(
				style.default_outlineColor);
			shape.setOutlineThickness(
				style.default_outlineThickness);
		}

		virtual void onMouseClickBegin()
		{
			shape.setFillColor(
				style.default_fillColor);
			shape.setOutlineColor(
				style.default_outlineColor);
			shape.setOutlineThickness(
				style.default_outlineThickness);
		}

		virtual void onMouseClickEnd()
		{
			shape.setFillColor(
				style.default_fillColor);
			shape.setOutlineColor(
				style.default_outlineColor);
			shape.setOutlineThickness(
				style.default_outlineThickness);
		}
	private:
		const Style style;

		bool checkIsInside(
			const int x,
			const int y) const override
		{
			return
				shape.getPosition().x >= x &&
				shape.getPosition().y >= y &&
				shape.getPosition().x + shape.getSize().x <= x &&
				shape.getPosition().y + shape.getSize().y <= y;
		}

		sf::RectangleShape shape;
	};
}
