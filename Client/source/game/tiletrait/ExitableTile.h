#pragma once

#include <Client/source/game/Environment.h>
#include <Client/source/game/tiletrait/GameTileBase.h>

#include <Client/source/game/GameEvent.h>

namespace Game
{
	// not possible with static tiles
	class ExitableTile
		:
		public RegisterableType
	{
	public:
		void registerType(Environment* const env)
		{
			env->registerTile<ExitableTile>(this);
		}

		GameEvent<ExitableTile> onExit;

	protected:
		void notifyExitEvent()
		{
			onExit.notify();
		}
	};
}
