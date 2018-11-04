#pragma once

#include <Client/source/menu/element/ElementBase.h>

#include <set>

namespace Menu
{
	class ContainerBase
		:
		public Base
	{
	public:
		virtual ~ContainerBase() = 0;

		virtual void onEvent(
			const sf::Event event)
		{
			for (ElementBase* const element : elements)
				element->onEvent(event);
		}

		virtual void onLogic(
			const sf::Time time)
		{
			for (ElementBase* const element : elements)
				if (element->isUseOnLogic())
				{
					element->onLogic(time);
				}
		}

		virtual void onDraw()
		{
			for (ElementBase* const element : elements)
				if (element->isUseOnEvent())
				{
					element->onDraw();
				}
		}

		virtual void addElement(
			ElementBase* const element)
		{
			elements.insert(element);
		}

		virtual void removeElement(
			ElementBase* const element)
		{
			elements.erase(element);
		}

		virtual void removeAllElements()
		{
			elements.clear();
		}
	protected:
		std::set<ElementBase*> elements;
	};
}