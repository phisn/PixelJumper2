#pragma once

#include <Client/source/menu/element/ElementBase.h>

#include <iterator>
#include <vector>

namespace Menu
{
	template <
		class Iterator
	>
	class ContainerBase
		:
		public ElementBase
	{
	public:
		typedef Iterator Iterator;
		typedef Properties Propeties;

		virtual ~ContainerBase() { }

		virtual void addElement(ElementBase* const element) = 0;
		virtual void clearElements() = 0;

		virtual Iterator begin() = 0;
		virtual Iterator end() = 0;

		virtual bool isEmpty() const = 0;
	};
}
