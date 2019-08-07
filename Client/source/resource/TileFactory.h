#pragma

#include <Client/source/game/tile/TileId.h>

#include <Client/source/resource/ResourceTileContentBase.h>
#include <Client/source/shared/tiles/TileWall.h>

#include <map>
#include <sal.h>

#define ADD_TILE(Name) Register(Game::TileId:: ## Name, []() -> TileBase* { return new Name ## Tile(); })

namespace Resource
{
	namespace TileFactory
	{
		typedef TileBase* (*Creator)();

		_Ret_maybenull_
		TileBase* Create(
			const Game::TileId Id);
		void Register(
			const Game::TileId Id,
			const Creator creator);

		static void RegisterAll()
		{
			// ADD_TILE(TileWall);
		}
	}
}
