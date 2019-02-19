#include "SynchronizableTile.h"

#include <Client/source/game/GameTileContainer.h>

void Game::SynchronizableTile::initialize(
	TileContainer* const container)
{
	container->insertSynchronizable(this);
}
