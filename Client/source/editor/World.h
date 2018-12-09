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
		struct TileGroup
		{
			const TileState* primaryState;
			sf::Vector2f size;
		};

		typedef std::vector<TileGroup> TileGroups;

		typedef std::vector<const TileState*> StateGroup;
		typedef std::vector<StateGroup> StateGroups;
	public:
		_Ret_maybenull_
		RESOURCE::World* convert() const
		{
			// init rand
			static bool triggered = false;

			if (!triggered)
			{
				triggered = true;
				srand(time(NULL));
			}
			// ---------

			RESOURCE::World* world = new RESOURCE::World();

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
			RESOURCE::World* const world) const
		{
			TileGroups tileGroups;

			{	StateGroups stateGroups;
			
				orderTiles(world, &stateGroups);
				groupTiles(&stateGroups, &tileGroups);
			} // delete stateGroups

			for (TileGroup& const tileGroup : tileGroups)
			{
				// convert to resource
			}
		}

		void orderTiles(
			RESOURCE::World* const world,
			StateGroups* const groups) const
		{
			for (const TileBase* const tile : tiles)
			{
			NEXT_TILE:
				const TileState* const state = tile->getState();

				for (StateGroup& group : *groups)
					if (group.back()->isSameGroup(state))
					{
						group.push_back(state);
						goto NEXT_TILE; // continue replacement
					}

				groups->emplace_back();
				groups->back().push_back(state);
			}
		}

		void groupTiles(
			StateGroups* const stateGroups,
			TileGroups* const tileGroups) const
		{
			for (const StateGroup& stateGroup : *stateGroups)
			{


				// group and push to tileGroups
			}
		}

		std::vector<TileBase*> tiles;
	};
}