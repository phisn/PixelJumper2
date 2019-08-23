#include "GameStateManager.h"

#include <set>

namespace
{
	std::set<Game::PlayerId> playerStateIds;
	std::set<Game::TileStateId> tileStateIds;

	bool hasWorldState = false;
}

namespace Game
{
	bool Game::GameStateManager::HasNextPlayerState()
	{
		return !playerStateIds.empty();
	}

	bool Game::GameStateManager::HasNextTileState()
	{
		return !tileStateIds.empty();
	}

	bool Game::GameStateManager::HasNextWorldState()
	{
		return hasWorldState;
	}

	PlayerId Game::GameStateManager::GetNextPlayerState()
	{
		const PlayerId psi = *tileStateIds.begin();
		tileStateIds.erase(psi);

		return psi;
	}

	TileStateId Game::GameStateManager::GetNextTileState()
	{
		const TileStateId tsi = *tileStateIds.begin();
		tileStateIds.erase(tsi);

		return tsi;
	}

	void Game::GameStateManager::SyncPlayerState(
		PlayerId playerStateId)
	{
		playerStateIds.insert(playerStateId);
	}

	void Game::GameStateManager::SyncTileState(
		TileStateId tileStateId)
	{
		tileStateIds.insert(tileStateId);
	}

	void Game::GameStateManager::SyncWorldState()
	{
		hasWorldState = true;
	}

	/* void Game::GameStateManager::SyncEverything()
	{
		SyncWorldState
	} */
}
