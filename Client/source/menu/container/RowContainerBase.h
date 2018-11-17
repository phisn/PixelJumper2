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
			useOnEvent = true;
		}

		bool initialize() override
		{
			if (!ContainerBase::initialize())
			{
				return false;
			}

			for (ElementBase* const element : elements)
				if (!element->initialize())
				{
					return false;
				}

			return true;
		}
		
		virtual void onEvent(
			const sf::Event event) override
		{
			for (ElementBase* const element : elements)
				// extend with if
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
				element->onDraw();
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
			element->setParent(this);
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
