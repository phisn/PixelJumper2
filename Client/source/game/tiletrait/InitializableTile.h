#pragma once

#include <Client/source/game/Environment.h>

namespace Game
{
	class WorldBase;
	class InitializableTile
		:
		public RegisterableType
	{
	public:
		void registerType(Environment* const env) override
		{
			env->registerTile<InitializableTile>(this);
		}

		virtual void initialize(WorldBase* const world) = 0;
	};
}
