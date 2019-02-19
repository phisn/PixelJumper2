#include "DynamicTile.h"

#include <Client/source/game/GameTileContainer.h>

void Game::DynamicTile::initialize(
	TileContainer* const container)
{
	container->insertDynamic(this);
}
