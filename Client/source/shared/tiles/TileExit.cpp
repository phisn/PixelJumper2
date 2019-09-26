#include "TileExit.h"

namespace Game
{
	void ExitTile::registerType(Environment* env)
	{
		CollidableTile::registerCollisionType(
			env,
			CollisionType::NormalCollision
		);
		ExitableTile::registerType(env);
		StaticTile::registerType(env);
	}
}
