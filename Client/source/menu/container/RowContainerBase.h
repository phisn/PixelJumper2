#pragma once

#include <Client/source/menu/container/ContainerBase.h>

#include <vector>

namespace Menu
{
	class RowContainerBase
		:
		public ContainerBase
	{
	public:
		RowContainerBase();
		~RowContainerBase();

		virtual void onEvent(
			const sf::Event event) override
		{

		}

		virtual void onLogic(
			const sf::Time time) override
		{

		}

		virtual void onDraw() override
		{

		}

		virtual void resetPosition() override
		{

		}

		void addElement(
			ElementBase* const element) override
		{

		}

		void clearElements() override
		{

		}

		bool isEmpty() const override
		{
			return elements.empty();
		}

	private:
		std::vector<ElementBase*> elements;
	};
}
