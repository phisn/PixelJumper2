#pragma once

#include <Client/source/menu/MenuNotifier.h>

namespace Game
{
	template <typename Parent, typename... Args>
	using GameEvent = Menu::MenuNotifier<Parent, Args...>;
}
