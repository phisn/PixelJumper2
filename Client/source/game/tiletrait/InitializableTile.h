#pragma once

#include <Client/source/game/GameWorld.h>

namespace Game
{
	class InitializableTile
		:
		public RegisterableType
	{
	public:
		void registerType(Environment* const env) override
		{
			env->registerTile<InitializableTile>(this);
		}

		// can fail
		virtual bool initialize(World* const world) = 0;
	};
}
