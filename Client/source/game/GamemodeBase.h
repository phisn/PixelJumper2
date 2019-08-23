#pragma once

#include <Client/source/game/VirtualPlayer.h>

#include <vector>

namespace Game
{
	class GamemodeBase
	{
	public:


	protected:
		std::vector<VirtualPlayer> players;
	};
}
