#pragma once

#include <Client/source/menu/MenuElementBase.h>
#include <Client/source/menu/MenuButtonMaterial.h>
#include <Client/source/menu/PercentProperty.h>

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

			void move(const sf::Vector2f offset)
			{
				rect.move(offset);
			}

			void draw(sf::RenderTarget* const target) const
			{
				target->draw(rect);
			}

		private:
			sf::Vector2f size;
			sf::Vector2f position;

			const CommonControlDirection direction;
			const CommonShapeStyle style = { sf::Color::Color(35, 35, 35), sf::Color::Color(), 0 };

			sf::RectangleShape rect;
		};
	}

	namespace SliderMaterial
	{
		struct Style : public CommonShapeStyle
		{
			float innerOffsetSide;
			float innerOffsetLimit;
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

			void move(const sf::Vector2f offset)
			{
				rect.move(offset);
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

			void draw(sf::RenderTarget* const target) const
			{
				target->draw(rect);
			}

		private:
			void updatePosition()
			{
				sf::Vector2f position = this->position + (direction == CommonControlDirection::Horizontal
					? sf::Vector2f(style->innerOffsetLimit, style->innerOffsetSide)
					: sf::Vector2f(style->innerOffsetSide, style->innerOffsetLimit));

				(direction == CommonControlDirection::Horizontal
					? position.x
					: position.y) += distance;

				rect.setPosition(position);
			}

			void updateSize()
			{
				sf::Vector2f size = this->size;

				(direction == CommonControlDirection::Horizontal
					? size.x
					: size.y) *= consumption;
				(direction == CommonControlDirection::Horizontal
					? size.y
					: size.x) -= style->innerOffsetSide * 2;
				(direction == CommonControlDirection::Horizontal
					? size.x
					: size.y) -= style->innerOffsetLimit * 2;

				rect.setSize(size);
			}

			const CommonControlDirection direction;

			const Style styles[3] =
			{
				{ sf::Color::Color(100, 100, 100), sf::Color::Color(), 0, 4, 4 },
				{ sf::Color::Color(150, 150, 150), sf::Color::Color(), 0, 4, 4 },
				{ sf::Color::Color(200, 200, 200), sf::Color::Color(), 0, 4, 4 }
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
				[this](const PercentProperty& distance)
				{
					// protect distance from NAN caused by converting to percent
					if (std::isnan(distance.getValue()))
					{
						this->distance->setValue(0.f);
					}

					sliderMaterial.setDistance(distance.getValue());
					onSliderMoved(distance.getValue());
				});

			length.addListener(
				[this](const PercentProperty& length)
				{
					// restore old distance as percent / relative
					static float oldLength = 0.f;
					distance->setPercent(
						getDistancePercent(distance->getValue(), oldLength)
					);

					sliderMaterial.setConsumption(length.getPercent());
					updateGraphics();

					oldLength = length.getValue();
				});
			limitDistance.addListener(
				[this](const bool oldValue,
					   const bool newValue)
				{
					distance->setValue(
						applyDistanceLimit(distance->getValue())
					);
				});
			limitOffset.addListener(
				[this](const float oldLimit,
					   const float limit)
				{
					distance->setValue(
						applyDistanceLimit(distance->getValue())
					);
				});
		}

		void initialize() override
		{
			onGraphicsDefault();
			ElementBase::initialize();
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
					this->distance->setValue(applyDistanceLimit(
						takeByDirection(
							event.mouseMove.x,
							event.mouseMove.y)
						- sliderPositionBegin
					));
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

		virtual void onDraw(sf::RenderTarget* const target) const
		{
			ElementBase::onDraw(target);

			barMaterial.draw(target);
			sliderMaterial.draw(target);
		}

		Property<PercentProperty> distance
		{
			[this](const float value) -> const float
			{ // convert to Value
				return value * (takeByDirection(
					this->size.getValue().x, 
					this->size.getValue().y) 
					
					- length->getValue() 
					- limitOffset.getValue() * 2);
			},
			[this](const float value) -> const float
			{ // convert to Percent
				return getDistancePercent(
					value,
					length->getValue());
			}
		};
		Property<PercentProperty> length
		{ 
			[this](const float value) -> const float
			{ // convert to Value
				return value * takeByDirection(
					this->size.getValue().x, 
					this->size.getValue().y);
			},
			[this](const float value) -> const float
			{ // convert to Percent
				return value / takeByDirection(
					this->size.getValue().x, 
					this->size.getValue().y);
			}
		};
		Property<bool> limitDistance{ true };
		Property<float> limitOffset{ 0.f };

		float convertPositionToPercent(const float position) const
		{
			return position / takeByDirection(
				size.getValue().x, 
				size.getValue().y);
		}

		float convertPercentToPosition(const float percent) const
		{
			return percent * takeByDirection(
				size.getValue().x, 
				size.getValue().y);
		}

		Menu::MenuNotifier<SlideBar, float> sliderPressed;
		Menu::MenuNotifier<SlideBar, float> sliderMoved;

	protected:
		virtual void updateOwnGraphics() override
		{
			barMaterial.setPosition(convertPositionVTR({ 0.f, 0.f }));

			sliderMaterial.setPosition(convertPositionVTR(
				limitDistance
				? direction == CommonControlDirection::Horizontal
				? sf::Vector2f(limitOffset, 0.f)
				: sf::Vector2f(0.f, limitOffset)
				: sf::Vector2f(0, 0)
			));

			barMaterial.setSize(size);
			sliderMaterial.setSize(size);
		}

		virtual void moveOwnGraphics(const sf::Vector2f offset) override
		{
			barMaterial.move(offset);
			sliderMaterial.move(offset);
		}

	private:
		virtual void onSliderPressed(const float distance)
		{
			sliderPressed.notify(distance);
		}

		virtual void onSliderMoved(const float distance)
		{
			sliderMoved.notify(distance);
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
		float getDistancePercent(
			const float value,
			const float length)
		{
			return value / (takeByDirection(
				this->size.getValue().x,
				this->size.getValue().y)

				- length
				- limitOffset.getValue() * 2);
		}

		float applyDistanceLimit(const float distance)
		{
			if (limitDistance)
			{
				if (distance < 0.f)
				{
					return 0.f;
				}
				else
				{
					const float limit = takeByDirection(
						this->size.getValue().x,
						this->size.getValue().y) 
						
						- length->getValue() 
						- limitOffset.getValue() * 2;

					if (distance > limit)
					{
						return limit;
					}
				}
			}

			return distance;
		}

		float takeByDirection(float x, float y) const
		{
			return direction == CommonControlDirection::Horizontal ? x : y;
		}

		bool sliderPressedInside = false;
		bool sliderMovedInside = false;

		float sliderPositionBegin = 0.f;
	};
}
