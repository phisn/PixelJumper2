#pragma once

#include <Client/source/game/GameState.h>
#include <Client/source/game/Environment.h>

namespace Game
{
	class SynchronizableTile
		:
		public RegisterableType
	{
	public:
		void registerType(Environment* const env) override
		{
			env->registerTile<SynchronizableTile>(this);
		}

		// currently not implemented
		virtual GameState* getState() = 0;
	};
}
