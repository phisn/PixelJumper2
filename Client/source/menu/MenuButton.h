#pragma once

#include <Client/source/device/ScreenDevice.h>
#include <Client/source/menu/MenuElementBase.h>

#include <SFML/Graphics.hpp>

namespace Menu
{
	namespace ButtonMaterial
	{
		class DefaultCircle
		{
		public:
			struct Style
			{
				sf::Color innerColor;
				sf::Color outerColor;

				int outlineThickness;
			};

			DefaultCircle()
			{
				applyEffect(CommonControlEffect::Default);
				rect.setPointCount(100);
			}

			bool contains(const sf::Vector2f point)
			{
				const float xDiff = point.x - (rect.getPosition().x + realSize.x / 2.f);
				const float yDiff = point.y - (rect.getPosition().y + realSize.y / 2.f);
				
				const float xDivR = (realSize.x * realSize.x) / 4.f;
				const float yDivR = (realSize.y * realSize.y) / 4.f;

				return (xDiff * xDiff) / xDivR + (yDiff * yDiff) / yDivR < 1;
			}

			void setPosition(const sf::Vector2f position)
			{
				rect.setPosition(position);
			}

			void setSize(const sf::Vector2f size)
			{
				rect.setRadius(
					std::min(size.x, size.y) / 2.f
				);
				rect.setScale(
					size.x < size.y ? 1 : size.x / size.y,
					size.y < size.x ? 1 : size.y / size.x
				);

				realSize = size;
			}

			void applyEffect(const CommonControlEffect effect)
			{
				rect.setFillColor(styles[(int)effect].innerColor);
				rect.setOutlineColor(styles[(int)effect].outerColor);
				rect.setOutlineThickness(styles[(int)effect].outlineThickness);
			}

			void draw() const
			{
				Device::Screen::Draw(rect);
			}

		private:
			sf::Vector2f realSize;

			Style styles[3] =
			{
				{ sf::Color::Color(50, 50, 50), sf::Color::Color(), 0 },
				{ sf::Color::Color(50, 50, 50), sf::Color::Color(100, 100, 100), 3 },
				{ sf::Color::Color(80, 80, 80), sf::Color::Color(), 0 }
			};

			sf::CircleShape rect;
		};

		class DefaultRectangle
		{
		public:
			struct Style
			{
				sf::Color innerColor;
				sf::Color outerColor;

				int outlineThickness;
			};

			DefaultRectangle()
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
				rect.setFillColor(styles[(int)effect].innerColor);
				rect.setOutlineColor(styles[(int)effect].outerColor);
				rect.setOutlineThickness(styles[(int)effect].outlineThickness);
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
	}

	template <typename Material = ButtonMaterial::DefaultRectangle>
	class Button
		:
		public ElementBase
	{
	public:
		virtual bool initialize() override
		{
			onGraphicsDefault();
			return ElementBase::initialize();
		}

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
					buttonPressedInside = true;

					onGraphicsPressed();
				}

				break;
			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left &&
					material.contains(sf::Vector2f(
						event.mouseButton.x,
						event.mouseButton.y))
					)
				{
					onButtonPressed();
					onGraphicsHovered();
				}
				else
				{
					onGraphicsDefault();
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
					buttonMovedInside = true;
					onGraphicsHovered();
				}
				else
				{
					if (buttonMovedInside)
					{
						onGraphicsDefault();
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

	protected:
		virtual void onButtonPressed() = 0;

		void updateOwnGraphics() override
		{
			material.setPosition(
				convertPositionVTR({ 0.f, 0.f })
			);
			material.setSize(size);
		}

		Material material;

		virtual void onGraphicsDefault()
		{
			material.applyEffect(CommonControlEffect::Default);
		}

		virtual void onGraphicsHovered()
		{
			material.applyEffect(CommonControlEffect::Hover);
		}

		virtual void onGraphicsPressed()
		{
			material.applyEffect(CommonControlEffect::Pressed);
		}

	private:
		bool buttonPressedInside = false;
		bool buttonMovedInside = false;
	};
}
