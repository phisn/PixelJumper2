#pragma once

#include <Client/source/game/GameTileBase.h>
#include <Client/source/game/Simulator.h>

namespace Game
{
	class EnterableTile
		:
		public GameTileBase
	{
	public:
		using GameTileBase::GameTileBase;

		// continue
		virtual bool onEnter(
			const PlayerEvent::EnterEvent event) = 0;
	};
}
