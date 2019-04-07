#pragma once

#include <Client/source/device/ScreenDevice.h>
#include <Client/source/menu/MenuElementBase.h>

#include <SFML/Graphics.hpp>

namespace Menu
{
	class DefaultButtonMaterial
	{
	public:
		struct Style
		{
			sf::Color innerColor;
			sf::Color outerColor;

			int outlineThickness;
		};

		DefaultButtonMaterial()
		{
			applyEffect(CommonControlEffect::Default);
		}

		bool contains(const sf::Vector2f point)
		{
			return rect.getGlobalBounds().contains(point);
		}

		void setPosition(const sf::Vector2f position)
		{
			rect.setPosition(position);
		}

		void setSize(const sf::Vector2f size)
		{
			rect.setSize(size);
		}

		void applyEffect(const CommonControlEffect effect)
		{
			rect.setFillColor(styles[(int) effect].innerColor);
			rect.setOutlineColor(styles[(int) effect].outerColor);
			rect.setOutlineThickness(styles[(int) effect].outlineThickness);
		}

		void draw() const
		{
			Device::Screen::Draw(rect);
		}

	private:
		Style styles[3] = 
		{
			{ sf::Color::Color(50, 50, 50), sf::Color::Color(), 0 },
			{ sf::Color::Color(50, 50, 50), sf::Color::Color(100, 100, 100), 3 },
			{ sf::Color::Color(80, 80, 80), sf::Color::Color(), 0 }
		};

		sf::RectangleShape rect;
	};

	template <typename Material = DefaultButtonMaterial>
	class Button
		:
		public ElementBase
	{
	public:
		virtual void onEvent(const sf::Event event)
		{
			ElementBase::onEvent(event);

			switch (event.type)
			{
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left &&
					material.contains(sf::Vector2f(
						event.mouseButton.x,
						event.mouseButton.y))
					)
				{
					material.applyEffect(CommonControlEffect::Pressed);
					buttonPressedInside = true;
				}

				break;
			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left &&
					material.contains(sf::Vector2f(
						event.mouseButton.x,
						event.mouseButton.y))
					)
				{
					material.applyEffect(CommonControlEffect::Hover);
					onButtonPressed();
				}
				else
				{
					material.applyEffect(CommonControlEffect::Default);
				}

				buttonPressedInside = false;

				break;
			case sf::Event::MouseMoved:
				if (buttonPressedInside)
				{
					break;
				}

				if (material.contains(sf::Vector2f(
						event.mouseMove.x,
						event.mouseMove.y))
					)
				{
					material.applyEffect(CommonControlEffect::Hover);
					buttonMovedInside = true;
				}
				else
				{
					if (buttonMovedInside)
					{
						material.applyEffect(CommonControlEffect::Default);
					}
				}

				break;
			}
		}

		virtual void onLogic(const sf::Time time)
		{
			ElementBase::onLogic(time);
		}

		virtual void onDraw() const
		{
			material.draw();

			ElementBase::onDraw();
		}

	private:
		bool buttonPressedInside = false;
		bool buttonMovedInside = false;

		virtual void onButtonPressed() = 0;
		virtual void onButtonHovered() = 0;
		virtual void onButtonDefault() = 0;

		void updateOwnGraphics() override
		{
			material.setPosition(
				convertPositionVTR({ 0.f, 0.f })
			);
			material.setSize(size);
		}

		Material material;
	};
}
