#pragma

#include <Client/source/game/tiles/TileId.h>

#include <Client/source/resource/tiles/TileBase.h>
#include <Client/source/resource/tiles/Wall.h>

#include <map>
#include <sal.h>

#define ADD_TILE(Name) Register(Game::Tile::Id:: ## Name, []() -> TileBase* { return new Name ## Tile(); })

namespace Resource
{
	namespace TileFactory
	{
		typedef TileBase* (*Creator)();

		_Ret_maybenull_
		TileBase* Create(
			const Game::Tile::Id ID);
		void Register(
			const Game::Tile::Id ID,
			const Creator creator);

		static void RegisterAll()
		{
			ADD_TILE(Wall);
		}
	}
}
