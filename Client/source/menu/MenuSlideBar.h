#pragma once

#include <Client/source/menu/MenuElementBase.h>
#include <Client/source/menu/MenuButtonMaterial.h>

namespace Menu
{
	namespace BarMaterial
	{
		class DefaultRectangle
		{
		public:
			DefaultRectangle()
			{
				rect.setFillColor(style.innerColor);
				rect.setOutlineColor(style.outerColor);
				rect.setOutlineThickness(style.outlineThickness);
			}

			void setPosition(const sf::Vector2f position)
			{
				rect.setPosition(position);
			}

			void setSize(const sf::Vector2f size)
			{
				rect.setSize(size);
			}

			void draw() const
			{
				Device::Screen::Draw(rect);
			}

		private:
			const CommonShapeStyle style = { sf::Color::Color(50, 50, 50), sf::Color::Color(), 0 };

			sf::RectangleShape rect;
		};
	}

	namespace SliderMaterial
	{
		class DefaultRectangle
		{
		public:
			DefaultRectangle(const CommonControlDirection direction)
				:
				direction(direction)
			{
				applyEffect(CommonControlEffect::Default);
			}

			bool contains(const sf::Vector2f point) const
			{
				return rect.getGlobalBounds().contains(point);
			}

			void setPosition(const sf::Vector2f position)
			{
				this->position = position;
				setDistance(distance);
			}

			void setSize(const sf::Vector2f size)
			{
				rect.setSize(size);
			}

			void setDistance(const float distance)
			{
				this->distance = distance;
				
				rect.setPosition(
					direction == CommonControlDirection::Horizontal 
					? position.x + distance
					: position.x,
					direction == CommonControlDirection::Horizontal
					? position.y
					: position.y + distance);
			}

			void applyEffect(const CommonControlEffect effect)
			{
				rect.setFillColor(
					styles[(int)effect].innerColor);
				rect.setOutlineColor(
					styles[(int)effect].outerColor);
				rect.setOutlineThickness(
					styles[(int)effect].outlineThickness);
			}

			void draw() const
			{
				Device::Screen::Draw(rect);
			}

		private:
			const CommonControlDirection direction;
			const CommonShapeStyle styles[3] =
			{
				{ sf::Color::Color(100, 100, 100), sf::Color::Color(), 0 },
				{ sf::Color::Color(150, 150, 150), sf::Color::Color(), 0 },
				{ sf::Color::Color(200, 200, 200), sf::Color::Color(), 0 }
			};

			float distance = 0.f;
			sf::Vector2f position;

			sf::RectangleShape rect;
		};
	}

	template <
		typename TBarMaterial,
		typename TSliderMaterial
	>
	class SlideBar
		:
		public ElementBase
	{
	public:
		SlideBar(const CommonControlDirection direction)
			:
			direction(direction),
			sliderMaterial(direction)
		{
			distance.addListener(
				[this](const float oldDistance,
					   const float newDistance)
				{
					updateGraphics();
				}
			);
		}

		bool initialize() override
		{
			return ElementBase::initialize();
		}

		void onEvent(const sf::Event event)
		{
			switch (event.type)
			{
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left &&
					sliderMaterial.contains(sf::Vector2f(
						event.mouseButton.x,
						event.mouseButton.y))
					)
				{
					sliderPressedInside = true;
					sliderPositionBegin = takeByDirection(
						event.mouseButton.x, 
						event.mouseButton.y);

					onGraphicsPressed();
				}

				break;
			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left &&
					sliderMaterial.contains(sf::Vector2f(
						event.mouseButton.x,
						event.mouseButton.y))
					)
				{
					onSliderPressed(
						takeByDirection(
							event.mouseButton.x,
							event.mouseButton.y)
						- sliderPositionBegin
					);
					onGraphicsHovered();
				}
				else
				{
					onGraphicsDefault();
				}

				sliderPressedInside = false;

				break;
			case sf::Event::MouseMoved:
				if (sliderPressedInside)
				{
					const float distance = takeByDirection(
						event.mouseMove.x,
						event.mouseMove.y)
						- sliderPositionBegin;

					sliderMaterial.setDistance(distance);
					onSliderMoved(distance);

					this->distance = distance;
				}
				else
				{
					if (sliderMaterial.contains(sf::Vector2f(
						event.mouseMove.x,
						event.mouseMove.y))
						)
					{
						sliderMovedInside = true;
						onGraphicsHovered();
					}
					else
					{
						if (sliderMovedInside)
						{
							onGraphicsDefault();
						}
					}

					break;
				}
			}
		}

		virtual void onLogic(const sf::Time time)
		{
			ElementBase::onLogic(time);
		}

		virtual void onDraw() const
		{
			barMaterial.draw();
			sliderMaterial.draw();

			ElementBase::onDraw();
		}

		Property<float> distance{ 0.f };

	private:
		virtual void onSliderPressed(const float distance) = 0;
		virtual void onSliderMoved(const float distance) = 0;

		void updateOwnGraphics() override
		{
			barMaterial.setPosition(
				convertPositionVTR({ 0.f, 0.f })
			);
			sliderMaterial.setPosition(
				convertPositionVTR(
					{ 
						0,
						0
					})
			);

			barMaterial.setSize(size);
			sliderMaterial.setSize(size);
		}

		const CommonControlDirection direction;

		TBarMaterial barMaterial;
		TSliderMaterial sliderMaterial;

		virtual void onGraphicsDefault()
		{
			sliderMaterial.applyEffect(CommonControlEffect::Default);
		}
		
		virtual void onGraphicsHovered()
		{
			sliderMaterial.applyEffect(CommonControlEffect::Hover);
		}

		virtual void onGraphicsPressed()
		{
			sliderMaterial.applyEffect(CommonControlEffect::Pressed);
		}

	private:
		float takeByDirection(float x, float y) const
		{
			return direction == CommonControlDirection::Horizontal ? x : y;
		}

		bool sliderPressedInside = false;
		bool sliderMovedInside = false;

		float sliderPositionBegin = 0.f;
	};
}
