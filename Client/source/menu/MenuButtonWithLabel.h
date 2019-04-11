#pragma once

#include <Client/source/menu/MenuLabel.h>
#include <Client/source/menu/MenuButton.h>

namespace Menu
{
	template <typename ButtonMaterial = ButtonMaterial::DefaultRectangle>
	class ButtonWithLabel
		:
		public Button<ButtonMaterial>
	{
	public:
		Label label;

		ButtonWithLabel()
		{
			position.addListener(
				[this](const sf::Vector2f oldPosition,
					   const sf::Vector2f newPosition)
				{
					updateLabelPosition();
				});
			size.addListener(
				[this](const sf::Vector2f oldSize,
					   const sf::Vector2f newSize)
				{   
					updateLabelPosition();
				});

			ElementBase::addStaticChild(&label);
		}

		Property<sf::Vector2f> labelOffset{ sf::Vector2f(0.f, -8.f) };
		
		void updateGraphics() override
		{
			updateLabelPosition();
			updateOwnGraphics();
		}

	private:
		void updateLabelPosition()
		{
			label.position = (size.getValue() - label.size.getValue()) / 2.f + labelOffset.getValue();
		}

		virtual void onGraphicsHovered()
		{
			Button::onGraphicsHovered();
			label.color = labelStyle.hover;
		}

		virtual void onGraphicsDefault()
		{
			Button::onGraphicsDefault();
			label.color = labelStyle.default;
		}

		virtual void onGraphicsPressed()
		{
			Button::onGraphicsPressed();
			label.color = labelStyle.pressed;
		}

		CommonControlStyle<sf::Color> labelStyle =
		{
			sf::Color::Color(255, 255, 255),
			sf::Color::Color(255, 255, 255),
			sf::Color::Color(190, 190, 190)
		};
	};
}