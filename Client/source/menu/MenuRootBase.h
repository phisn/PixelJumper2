#pragma once

#include <Client/source/device/ScreenDevice.h>
#include <Client/source/menu/MenuElementBase.h>

#include <SFML/Graphics/View.hpp>

namespace Menu
{
	class RootBase
		:
		private ElementBase
	{
	public:
		using ElementBase::initialize;
		using ElementBase::onEvent;
		using ElementBase::onLogic;
		using ElementBase::onDraw;
		using ElementBase::innerOffset;

		using ElementBase::size;
		using ElementBase::position;

		RootBase()
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

		void onDraw() const
		{
			Device::Screen::SetView(view);
			ElementBase::onDraw();
		}

	private:
		void updateOwnGraphics() override
		{
		}

		sf::View view;
	};
}
