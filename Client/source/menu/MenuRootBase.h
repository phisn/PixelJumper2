#pragma once

#include <Client/source/device/ScreenDevice.h>
#include <Client/source/menu/MenuElementBase.h>

#include <SFML/Graphics/View.hpp>

namespace Menu
{
	class MenuRootBase
		:
		protected ElementBase
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
					const sf::FloatRect realView = ConvertPortToReal(newViewPort);

					size = sf::Vector2f(
						realView.width,
						realView.height
					);
					position = sf::Vector2f(
						realView.left,
						realView.top
					);

					view.setSize(
						viewPort->width * viewSize->x,
						viewPort->height * viewSize->y
					);

					view.setCenter(*size / 2.f + *position);
				});
			viewSize.addListener(
				[this](const sf::Vector2f oldSize,
					   const sf::Vector2f newSize)
				{
					view.setSize(
						viewPort->width * newSize.x,
						viewPort->height * newSize.y
					);
				});

			viewPort = { 0.f, 0.f, 1.f, 1.f };
			viewSize = sf::Vector2f(
				Device::Screen::GetWindow()->getSize()
			);
		}

		void addElement(ElementBase* const element)
		{
			addChild(element);
		}

		Property<sf::FloatRect> viewPort;
		Property<sf::Vector2f> viewSize{ sf::Vector2f(0.f, 0.f) };

		const sf::View& readView() const
		{
			return view;
		}

		void draw() const
		{
			Device::Screen::SetView(view);
			onDraw();
			Device::Screen::ResetView();
		}

		void update()
		{
			updateGraphics();
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

		void updateSizeWithSpace() override
		{
		}

	protected:
		sf::View view;

		std::vector<int> customStrongEvents; // TODO: realize
		std::vector<int> customWeakEvents;
	};
}
