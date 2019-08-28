#pragma once

#include <Client/source/menu/MenuSpecialProperties.h>

namespace Game
{
	template <typename T>
	using Property = Menu::LazyProperty<T>;
	template <typename T, typename Parent>
	using ReadOnlyProperty = Menu::ReadOnlyPropertyContainer<Property<T>, Parent>;
}
