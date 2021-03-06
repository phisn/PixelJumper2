#pragma once

#include <Client/source/device/ScreenDevice.h>

#include <Client/source/menu/MenuElementBase.h>
#include <Client/source/menu/MenuNotifier.h>

#include <SFML/Graphics.hpp>

namespace Menu
{
	template <typename TMaterial>
	class Button
		:
		public ElementBase
	{
	public:
		virtual void initialize() override
		{
			onGraphicsDefault();
			ElementBase::initialize();
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
					if (buttonPressedInside)
					{
						onButtonPressed();
					}

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

		MenuNotifier<Button> buttonPressedEvent;

		virtual void onLogic(const sf::Time time)
		{
			ElementBase::onLogic(time);
		}

		virtual void onDraw(sf::RenderTarget* const target) const
		{
			material.draw(target);

			ElementBase::onDraw(target);
		}

	protected:
		virtual void updateOwnGraphics() override
		{
			material.setPosition(convertPositionVTR({ 0.f, 0.f }));
			material.setSize(size);
		}

		virtual void moveOwnGraphics(const sf::Vector2f offset) override
		{
			material.move(offset);
		}

		virtual void onButtonPressed()
		{
			buttonPressedEvent.notify();
		}

		TMaterial material;

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
