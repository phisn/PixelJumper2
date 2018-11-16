#pragma once

#include <Client/source/menu/Common.h>
#include <Client/source/menu/container/ContainerBase.h>

#include <vector>

namespace Menu
{
	class RowContainerBase
		:
		public ContainerBase<std::vector<ElementBase*>::iterator>
	{
		typedef std::vector<ElementBase*> Container;
	public:
		struct Style
		{
			float elementMargin;
		};
		
		RowContainerBase(
			const Style style,
			const Direction direction)
		{
		}
		
		virtual void onEvent(
			const sf::Event event) override
		{
			for (ElementBase* const element : elements)
				element->onEvent(event);
		}

		virtual void onLogic(
			const sf::Time time) override
		{
			for (ElementBase* const element : elements)
				if (element->isUseOnLogic())
				{
					element->onLogic(time);
				}
		}

		virtual void onDraw() override
		{
			for (ElementBase* const element : elements)
				if (element->isUseOnEvent())
				{
					element->onDraw();
				}
		}

		virtual void resetPosition() override
		{
			ContainerBase::resetPosition();

			for (ElementBase* const element : elements)
				element->resetPosition();
		}

		void addElement(
			ElementBase* const element) override
		{
			elements.push_back(element);
		}

		void clearElements() override
		{
			elements.clear();
		}

		Container::iterator begin() override
		{
			return elements.begin();
		}

		Container::iterator end() override
		{
			return elements.end();
		}

		bool isEmpty() const override
		{
			return elements.empty();
		}

	private:
		Container elements;

		float elementOffset = 0.f;
	};
}
