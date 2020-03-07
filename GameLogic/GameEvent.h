#pragma once

#include <Client/source/menu/MenuNotifier.h>

namespace Game
{
	struct GameEventIdentifier
	{
		enum
		{
			ClassicSimulation = 17
		};
	};

	template <typename Parent, typename... Args>
	using GameEvent = Menu::MenuNotifier<Parent, Args...>;
}
