#pragma once

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

		RootBase()
		{
			viewPort.addListener(
				[this](const sf::FloatRect oldViewPort, 
					   const sf::FloatRect newViewPort)
				{
					view.setViewport(newViewPort);
					size = view.getSize();
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

	private:
		void updateOwnGraphics() override
		{
		}

		sf::View view;
	};
}
