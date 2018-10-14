#pragma once

#include <Client/source/scene/GameBaseScene.h>

namespace Scene
{
	class LocalGame
		:
		public GameBase
	{
	public:
		LocalGame(Game::World* world)
			:
			GameBase(world)
		{
		}
	};
}
