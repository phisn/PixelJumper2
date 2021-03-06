#pragma once

#include <Client/source/logger/Logger.h>

#include <Client/source/editor/tile/TileTemplate.h>

#include <Client/source/editor/manipulator/Executor.h>
#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/EditorWorld.h>
#include <Client/source/editor/manipulator/Manipulator.h>

#include <Client/source/resource/ResourceInterface.h>
#include <Client/source/resource/WorldResource.h>

#include <string>

namespace Editor
{
	class LoadWorldTask
		:
		public Task
	{
	public:
		LoadWorldTask(const std::wstring name)
			:
			Task(),
			name(name)
		{
		}
		
		~LoadWorldTask()
		{
			for (TileBase* tile : tiles)
			{
				delete tile;
			}
		}

		bool execute(World* const world) override
		{
			Log::Section section(L"Loading world '" + name + L"'");

			Resource::World worldBuffer;
			if (!Resource::Interface::LoadResource(
					&worldBuffer,
					Resource::ResourceType::World,
					name))
			{
				section.error(L"Failed to read resource");

				return false;
			}

			for (TileBase* const tile : world->getTiles())
			{
				tiles.push_back(tile);
			}

			for (TileBase* const tile : tiles)
			{
				world->removeTile(tile);
			}

			if (!world->adopt(&worldBuffer))
			{
				undo(world);
				tiles.clear();

				section.error(L"Failed to adopt resource");

				return false;
			}

			Manipulator::GetCache()->selection.notify();
			return true;
		}

		void redo(World* const world) override
		{
			undo(world); // same
		}

		void undo(World* const world) override
		{
			std::vector<TileBase*> temp;
			for (TileBase* const tile : world->getTiles())
			{
				temp.push_back(tile);
			}

			for (TileBase* const tile : temp)
			{
				world->removeTile(tile);
			}

			for (TileBase* const tile : tiles)
			{
				world->setTileUnsafe(tile);
			}
			tiles = temp;
			Manipulator::GetCache()->selection.notify();
		}

	private:
		std::vector<TileBase*> tiles;

		const std::wstring name;
	};
}
