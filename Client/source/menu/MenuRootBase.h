#pragma once

#include <Client/source/device/ScreenDevice.h>
#include <Client/source/menu/MenuElementBase.h>

#include <SFML/Graphics/View.hpp>

namespace Menu
{
	class MenuRootBase
		:
		private ElementBase
	{
	public:
		void initialize() override
		{
			ElementBase::initialize();
			updateGraphics();
		}

		using ElementBase::initialize;
		using ElementBase::onEvent;
		using ElementBase::onLogic;
		using ElementBase::onDraw;
		using ElementBase::innerOffset;

		using ElementBase::size;
		using ElementBase::position;

		MenuRootBase()
		{
			viewPort.addListener(
				[this](const sf::FloatRect oldViewPort, 
					   const sf::FloatRect newViewPort)
				{
					view.setViewport(newViewPort);

					const sf::Vector2u windowSize = Device::Screen::GetWindow()->getSize();

					size = sf::Vector2f(
						windowSize.x * newViewPort.width,
						windowSize.y * newViewPort.height
					);
					position = sf::Vector2f(
						windowSize.x * newViewPort.left,
						windowSize.y * newViewPort.top
					);

					view.setSize(size.getValue());
					view.setCenter(size.getValue() / 2.f + position.getValue());
				});

			viewPort = { 0.f, 0.f, 1.f, 1.f };
		}

		void addElement(ElementBase* const element)
		{
			addStaticChild(element);
		}

		Property<sf::FloatRect> viewPort;

		const sf::View& readView() const
		{
			return view;
		}

		void onDraw() const override
		{
			Device::Screen::SetView(view);
			ElementBase::onDraw();
			Device::Screen::ResetView();
		}

		void onEvent(sf::Event event) override
		{
			switch (event.type)
			{
			case sf::Event::MouseMoved:
			{
				const sf::Vector2f position = Device::Screen::GetWindow()->mapPixelToCoords(sf::Vector2i(
					event.mouseMove.x,
					event.mouseMove.y),
					view);

				event.mouseMove.x = position.x;
				event.mouseMove.y = position.y;
			}
				break;
			case sf::Event::MouseButtonPressed:
			{
				const sf::Vector2f position = Device::Screen::GetWindow()->mapPixelToCoords(sf::Vector2i(
					event.mouseButton.x,
					event.mouseButton.y),
					view);

				event.mouseButton.x = position.x;
				event.mouseButton.y = position.y;
			}
				break;
			case sf::Event::MouseButtonReleased:
			{
				const sf::Vector2f position = Device::Screen::GetWindow()->mapPixelToCoords(sf::Vector2i(
					event.mouseButton.x,
					event.mouseButton.y),
					view);

				event.mouseButton.x = position.x;
				event.mouseButton.y = position.y;
			}
				break;
			}

			ElementBase::onEvent(event);
		}

		static sf::FloatRect ConvertRealToPort(const sf::FloatRect rect)
		{
			const sf::Vector2u windowSize = Device::Screen::GetWindow()->getSize();

			return
			{
				rect.left / windowSize.x,
				rect.top / windowSize.y,
				rect.width / windowSize.x,
				rect.height / windowSize.y
			};
		}

		static sf::FloatRect ConvertPortToReal(const sf::FloatRect rect)
		{
			const sf::Vector2u windowSize = Device::Screen::GetWindow()->getSize();

			return
			{
				rect.left * windowSize.x,
				rect.top * windowSize.y,
				rect.width * windowSize.x,
				rect.height * windowSize.y
			};
		}

		// called in Scene::onCreate
		// to build the super ui
		virtual bool build() = 0;

	protected:
		virtual void updateOwnGraphics() override
		{
		}

		sf::View view;
	};
}
