#pragma once

#include <Client/source/game/tiletrait/CollidableTile.h>
#include <Client/source/game/tiletrait/StaticTile.h>

namespace Game
{
	class WallTile
		:
		public StaticTile,
		public CollidableTile
	{
	public:
		void initialize(TileContainer* const container) override
		{
			CollidableTile::addCollisionType(
				container,
				CollisionType(
					false,
					false,
					false)
			);
			StaticTile::initialize(container);
		}
	};
}
