#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/editor/tile/TileBase.h>
#include <Client/source/editor/template/TileTemplate.h>

#include <Client/source/resource/WorldResource.h>

#include <vector>

namespace Editor
{
	class World
	{
		// group = NxN area (of tiles)
		struct ExtendedTile
		{
			const TileState* primaryState;

			sf::Vector2f position, size;
		};

		typedef std::vector<ExtendedTile> ExtendedTiles;

		// Tile = TileGroup
		typedef ExtendedTile Tile;

		typedef std::vector<Tile> TileGroup;
		typedef std::vector<TileGroup> TileGroups;
	public:
		_Ret_maybenull_
		RESOURCE::_N_World* convert() const
		{
			// init rand
			static bool triggered = false;

			if (!triggered)
			{
				triggered = true;
				srand(time(NULL));
			}
			// ---------

			RESOURCE::_N_World* world = new RESOURCE::_N_World();

			world->header.begin.magic = WORLD_MAGIC;
			world->header.begin.worldId = rand();

			saveTiles(world);

			// TODO: Setup header and content

			if (world->validateHeader() && world->validateContent())
			{
				return world;
			}

			return NULL;
		}

	private:
		void saveTiles(
			RESOURCE::_N_World* const world) const
		{
			ExtendedTiles tileGroups;

			{	TileGroups stateGroups;
			
				orderTiles(world, &stateGroups);
				groupTiles(&stateGroups, &tileGroups);
			} // delete stateGroups

			for (ExtendedTile& const extendedTile : tileGroups)
			{
				world->content.tileResources.push_back(
					extendedTile.primaryState->create(
						extendedTile.size,
						extendedTile.position);
				);

				// convert to resource
			}
		}

		void orderTiles(
			RESOURCE::_N_World* const world,
			TileGroups* const groups) const
		{
			for (const TileBase* const tile : tiles)
			{
			NEXT_TILE:
				const TileState* const state = tile->getState();

				for (TileGroup& group : *groups)
					if (group.back().primaryState->isSameGroup(state))
					{
						pushTile(
							tile, 
							state,
							&group
						);

						goto NEXT_TILE; // continue replacement
					}

				groups->emplace_back();
				pushTile(
					tile, 
					state, 
					&groups->back()
				);
			}
		}

		void pushTile(
			const TileBase* const tile,
			const TileState* const state,

			TileGroup* const group) const
		{
			group->emplace_back();

			group->back().position = tile->getShape()->getPosition();
			group->back().primaryState = state;
			// ignore size
		}

		void groupTiles(
			TileGroups* const stateGroups,
			ExtendedTiles* const extendedTiles) const
		{
			for (const TileGroup& tileGroup : *stateGroups)
			{
				// for tests replace later
				for (const Tile& tile : tileGroup)
				{
					extendedTiles->emplace_back();
					extendedTiles->back() = tile;
				}
				// -----------------------

				// group and push to tileGroups
			}
		}

		std::vector<TileBase*> tiles;
	};
}