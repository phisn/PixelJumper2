#pragma once

#include "Common/Notifier.h"

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
	using GameEvent = Util::Notifier<Parent, Args...>;
}
