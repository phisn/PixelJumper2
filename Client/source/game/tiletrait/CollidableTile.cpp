#include "CollidableTile.h"

#include <Client/source/game/GameTileContainer.h>

void Game::CollidableTile::addCollisionType(
	TileContainer* const container, 
	const CollisionType type)
{
	container->insertCollidable(this, type);
}
