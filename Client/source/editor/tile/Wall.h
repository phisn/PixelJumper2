#pragma once

#include <Client/source/editor/tile/TileBase.h>

#include <Client/source/game/tiles/Wall.h>

namespace Editor
{
	namespace Tile
	{
		class Wall
			:
			public TileBase
		{
			typedef TileState WallState;
		public:
			Wall(
				const sf::Vector2f position)
				:
				TileBase(
					TILE_WALL_COLOR,
					position)
			{
			}

			TileState* getState() const
			{
				WallState* state = new WallState();
				state->position = getShape()->getPosition();
				return (TileState*)state;
			}


		};
	}
}