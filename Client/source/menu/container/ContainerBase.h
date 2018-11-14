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
		typedef Properties Propeties;


		virtual ~ContainerBase() { }

		virtual void addElement(ElementBase* const element) = 0;
		virtual void clearElements() = 0;

		virtual bool isEmpty() const = 0;
	};

	class _ContainerBase
		:
		public ElementBase
	{
	public:
		typedef Properties Properties;

		_ContainerBase(
			ElementBase* const parent)
			:
			ElementBase(parent)
		{
		}

		virtual ~_ContainerBase() { }

		virtual void addElement(ElementBase* const element) = 0;
		virtual void clearElements() = 0;

		virtual bool isEmpty() const = 0;
	};
}
