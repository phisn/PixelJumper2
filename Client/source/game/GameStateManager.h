#pragma once

#include <Client/source/game/PlayerState.h>
#include <Client/source/game/WorldState.h>

namespace Game
{
	enum class GameStateType
	{
		PlayerState,
		TileState,
		WorldState
	};

	namespace GameStateManager
	{
		bool HasNextPlayerState();
		bool HasNextTileState();
		bool HasNextWorldState();

		PlayerId GetNextPlayerState();
		TileStateId GetNextTileState();

		void SyncPlayerState(PlayerId playerStateId);
		void SyncTileState(TileStateId tileStateId);
		void SyncWorldState();

		void SyncEverything(); // :d
	}
}
