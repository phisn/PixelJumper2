#pragma once

#include <Client/source/menu/MenuElementBase.h>
#include <Client/source/menu/MenuButtonMaterial.h>
#include <Client/source/menu/PercentValue.h>

#include <functional>

namespace Menu
{
	namespace BarMaterial
	{
		// TODO: Correct position
		class DefaultRectangle
		{
		public:
			DefaultRectangle(const CommonControlDirection direction)
				:
				direction(direction)
			{
				rect.setFillColor(style.innerColor);
				rect.setOutlineColor(style.outerColor);
				rect.setOutlineThickness(style.outlineThickness);
			}

			void setPosition(sf::Vector2f position)
			{
				this->position = position;
				
				/*(direction == CommonControlDirection::Horizontal
					? position.y
					: position.x) +=
					(
						(direction == CommonControlDirection::Horizontal
							? size.y
							: size.x) / 2.f
						- 
						(direction == CommonControlDirection::Horizontal
							? size.y
							: size.x) / 8.f
					);*/

				rect.setPosition(position);
			}

			void setSize(sf::Vector2f size)
			{
				this->size = size;

				/*(direction == CommonControlDirection::Horizontal
					? size.y
					: size.x) /= 4.f;*/

				rect.setSize(size);
				setPosition(position);
			}

			void draw() const
			{
				Device::Screen::Draw(rect);
			}

		private:
			sf::Vector2f size;
			sf::Vector2f position;

			const CommonControlDirection direction;
			const CommonShapeStyle style = { sf::Color::Color(50, 50, 50), sf::Color::Color(), 0 };

			sf::RectangleShape rect;
		};
	}

	namespace SliderMaterial
	{
		struct Style : public CommonShapeStyle
		{
			float innerOffset;
		};

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
				updatePosition();
			}

			void setSize(const sf::Vector2f size)
			{
				this->size = size;
				updateSize();
			}

			void setConsumption(const float consumption)
			{
				this->consumption = consumption;
				updateSize();
			}

			void setDistance(const float distance)
			{
				this->distance = distance;
				updatePosition();
			}

			void applyEffect(const CommonControlEffect effect)
			{
				style = styles + (int) effect;

				rect.setFillColor(style->innerColor);
				rect.setOutlineColor(style->outerColor);
				rect.setOutlineThickness(style->outlineThickness);

				updateSize();
				updatePosition();
			}

			void draw() const
			{
				Device::Screen::Draw(rect);
			}

		private:
			void updatePosition()
			{
				sf::Vector2f position = this->position + sf::Vector2f(
					style->innerOffset, 
					style->innerOffset);

				(direction == CommonControlDirection::Horizontal
					? position.x
					: position.y) += distance;

				rect.setPosition(position);
			}

			void updateSize()
			{
				sf::Vector2f size = this->size;

				(direction == CommonControlDirection::Horizontal
					? size.y
					: size.x) -= style->innerOffset * 2;
				(direction == CommonControlDirection::Horizontal
					? size.x
					: size.y) *= consumption;

				rect.setSize(size);
			}

			const CommonControlDirection direction;

			const Style styles[3] =
			{
				{ sf::Color::Color(100, 100, 100), sf::Color::Color(), 0, 4 },
				{ sf::Color::Color(150, 150, 150), sf::Color::Color(), 0, 4 },
				{ sf::Color::Color(200, 200, 200), sf::Color::Color(), 0, 4 }
			};
			const Style* style = styles;

			float distance = 0.f, consumption;
			sf::Vector2f position, size;

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
			sliderMaterial(direction),
			barMaterial(direction)
		{
			distance.addListener(
				[this](const PercentValue& distance)
				{
					float value = distance.getValue();

					if (limitDistance.getValue())
					{
						const float offset = limitOffset.getValue();
						const float limit = takeByDirection(
							this->size.getValue().x, 
							this->size.getValue().y) - offset - length->getValue();

						value = value < offset
							? offset
							: value > limit
							? limit
							: value;
					}

					sliderMaterial.setDistance(value);
					onSliderMoved(value);
					
					updateGraphics();
				});
			length.addListener(
				[this](const PercentValue& length)
				{
					sliderMaterial.setConsumption(length.getPercent());
					updateGraphics();
				});
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
						event.mouseButton.y) - distance->getValue();

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

					this->distance->setValue(distance);
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

		Property<PercentValue> distance
		{
			[this](const float value) -> const float
			{ // convert to Value
				return value * (takeByDirection(this->size.getValue().x, this->size.getValue().y) - length->getValue() - limitOffset.getValue());
			},
			[this](const float value) -> const float
			{ // convert to Percent
				return value / (takeByDirection(this->size.getValue().x, this->size.getValue().y) - length->getValue() - limitOffset.getValue());
			}
		};
		Property<PercentValue> length
		{ 
			[this](const float value) -> const float
			{ // convert to Value
				return value * takeByDirection(this->size.getValue().x, this->size.getValue().y);
			},
			[this](const float value) -> const float
			{ // convert to Percent
				return value / takeByDirection(this->size.getValue().x, this->size.getValue().y);
			}
		};
		Property<bool> limitDistance{ true };
		Property<float> limitOffset{ 0.f };

		float convertPositionToPercent(const float position) const
		{
			return position / takeByDirection(size.getValue().x, size.getValue().y);
		}

		float convertPercentToPosition(const float percent) const
		{
			return percent * takeByDirection(size.getValue().x, size.getValue().y);
		}

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
