#pragma once

namespace Game
{
	// not possible with static tiles
	class DrawableTile
	{
	public:
		virtual void onDraw() = 0;
	};
}
