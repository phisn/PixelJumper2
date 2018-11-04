#pragma once

#include <Client/source/menu/MenuBase.h>

#include <set>

namespace Menu
{
	class ChildContainer
		:
		public Base
	{
	public:
		virtual ~ChildContainer() = 0;

		virtual void onEvent(
			const sf::Event event)
		{
			for (Base* const element : elements)
				element->onEvent(event);
		}

		virtual void onLogic(
			const sf::Time time)
		{
			for (Base* const element : elements)
				if (element->isUseOnLogic())
				{
					element->onLogic(time);
				}
		}

		virtual void onDraw()
		{
			for (Base* const element : elements)
				if (element->isUseOnEvent())
				{
					element->onDraw();
				}
		}

		virtual void addElement(
			Base* const element)
		{
			elements.insert(element);
		}

		virtual void removeElement(
			Base* const element)
		{
			elements.erase(element);
		}

		virtual void removeAllElements()
		{
			elements.clear();
		}
	protected:
		std::set<Base*> elements;
	};
}