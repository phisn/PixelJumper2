#pragma once

#include <Client/source/game/GameTileContainer.h>

namespace Game
{
	// not possible with static tiles
	class DrawableTile
	{
	public:


		virtual void onDraw() = 0;
	};
}
