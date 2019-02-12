#pragma once

#include <Client/source/game/GameTileBase.h>
#include <Client/source/game/Simulator.h>

namespace Game
{
	class LeaveableTile
		:
		public GameTileBase
	{
	public:
		using GameTileBase::GameTileBase;

		virtual void onLeave(
			const PlayerEvent::LeaveEvent event) = 0;
	};
}
