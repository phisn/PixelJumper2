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
		return false;
	}
	bool Game::GameStateManager::HasNextTileState()
	{
		return false;
	}

	bool Game::GameStateManager::HasNextWorldState()
	{
		return false;
	}

	PlayerId Game::GameStateManager::GetNextPlayerState()
	{
		return PlayerId();
	}

	TileStateId Game::GameStateManager::GetNextTileState()
	{
		return TileStateId();
	}

	void Game::GameStateManager::SyncPlayerState(PlayerId playerStateId)
	{
	}

	void Game::GameStateManager::SyncTileState(TileStateId tileStateId)
	{
	}

	void Game::GameStateManager::SyncWorldState()
	{
	}

	void Game::GameStateManager::SyncEverything()
	{
	}
}
