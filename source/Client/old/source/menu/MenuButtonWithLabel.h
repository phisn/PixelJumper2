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
		Label value;

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
					value.sizePreferred = { newSize.x * labelDependentSize->x, newSize.y * labelDependentSize->y };
				});
			horizontalArea.addListener(
				[this](const CommonHorizontalArea oldArea,
					   const CommonHorizontalArea newArea)
				{
					updateLabelPosition();
				});
			verticalArea.addListener(
				[this](const CommonVerticalArea oldArea,
					   const CommonVerticalArea newArea)
				{
					updateLabelPosition();
				});

			ElementBase::addChild(&value);
		}

		Property<
			CommonControlStyle<sf::Color>
		> labelStyle = CommonControlStyle<sf::Color>{
			sf::Color::Color(255, 255, 255),
			sf::Color::Color(255, 255, 255),
			sf::Color::Color(190, 190, 190)
		};
		Property<sf::Vector2f> labelDependentSize{ sf::Vector2f(1.f, 1.f) };

		Property<sf::Vector2f> labelOffset{ sf::Vector2f(0.f, 0) };

	protected:
		virtual void updateOwnGraphics() override
		{
			Button::updateOwnGraphics();
		}

		virtual void moveOwnGraphics(const sf::Vector2f offset) override
		{
			Button::moveOwnGraphics(offset);
		}

	private:
		void updateLabelPosition()
		{
			sf::Vector2f position = { };

			if (horizontalArea != CommonHorizontalArea::Left)
			{
				position.x = (size->x - value.size->x);

				if (horizontalArea == CommonHorizontalArea::Center)
				{
					position.x /= 2.f;
				}
			}

			if (verticalArea != CommonVerticalArea::Top)
			{
				position.y = (size->y - value.size->y);

				if (verticalArea == CommonVerticalArea::Center)
				{
					position.y /= 2.f;
				}
			}

			value.position = position + *labelOffset;
		}

		virtual void onGraphicsHovered() override
		{
			Button::onGraphicsHovered();
			value.color = labelStyle->hover;
		}

		virtual void onGraphicsDefault() override
		{
			Button::onGraphicsDefault();
			value.color = labelStyle->default;
		}

		virtual void onGraphicsPressed() override
		{
			Button::onGraphicsPressed();
			value.color = labelStyle->pressed;
		}
	};
}
