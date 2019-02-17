#include "StaticTile.h"

#include <Client/source/game/GameTileContainer.h>

void Game::StaticTile::initialize(
	TileContainer* const container)
{
	container->insertStatic(this);
}
