#pragma once

#include <Client/source/menu/element/ElementBase.h>

#include <vector>

namespace Menu
{
	class ContainerBase
		:
		public ElementBase
	{
	public:
		typedef Properties Properties;

		ContainerBase(
			ElementBase* const parent)
			:
			ElementBase(parent)
		{
		}

		virtual ~ContainerBase() = 0;

		virtual void addElement(ElementBase* const element) = 0;
		virtual void clearElements() = 0;

		virtual bool isEmpty() const = 0;
	};
}
