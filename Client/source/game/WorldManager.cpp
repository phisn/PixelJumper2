#include "WorldManager.h"

namespace Game
{
	RESOURCE::World*WorldManager::loadResourceFromFile(
		const std::wstring fileName)
	{
		std::ifstream file(
			fileName,
			std::ios::binary | std::ios::in | std::ios::ate);

		if (!file)
		{
			return NULL;
		}

		const sf::Uint32 size = file.tellg();
		file.clear();
		file.seekg(0, std::ios::beg);

		RESOURCE::ByteBuffer buffer(size);
		file.read(
			buffer.use(size),
			size);

		if (file.fail())
		{
			return NULL;
		}

		file.close();

		RESOURCE::ByteReader reader(&buffer);
		RESOURCE::World* world = new RESOURCE::World();

		if (!world->readFromBuffer(&reader))
		{
			delete world;
			return NULL;
		}

		return world;
	}
	
	bool WorldManager::saveResourceToFile(
		const std::wstring fileName, 
		RESOURCE::World* world)
	{
		std::ofstream file(
			fileName,
			std::ios::binary | std::ios::out);

		if (!file)
		{
			return false;
		}

		RESOURCE::ByteWriter writer;
		if (!world->writeToBuffer(&writer))
		{
			return false;
		}

		file.write(
			writer.readBuffer()->read(),
			writer.getLength());

		if (file.fail())
		{
			return false;
		}

		file.close();

		return true;
	}
	RESOURCE::World* WorldManager::convertSettingsToResource(
		WorldSettings* settings)
	{
		RESOURCE::World* world = new RESOURCE::World();

		world->header.prop.width = settings->size.x;
		world->header.prop.height = settings->size.y;

		world->header.auth.author = settings->author;
		world->header.auth.name = settings->name;

		for (Tile::Base* tile : settings->tiles)
		{
			world->content.tileResources.emplace_back();
			world->content.tileResources.back().tile = tile;
		}

		return world;
	}

	WorldSettings* WorldManager::convertResourceToSettings(
		RESOURCE::World* world)
	{
		WorldSettings* settings = new WorldSettings();

		settings->size = sf::Vector2f(
			world->header.prop.width,
			world->header.prop.height);

		settings->author = world->header.auth.author;
		settings->name = world->header.auth.name;

		for (Resource::Tile& tile : world->content.tileResources)
		{
			settings->tiles.push_back(tile.tile);
		}

		return settings;
	}

	WorldSettings* WorldManager::loadSettingsFromFile(
		const std::wstring fileName)
	{
		RESOURCE::World* world = loadResourceFromFile(
			fileName);

		if (!world)
		{
			return NULL;
		}

		WorldSettings* settings = convertResourceToSettings(
			world);
		delete world;

		return settings;
	}

	bool WorldManager::saveSettingsToFile(
		const std::wstring fileName, 
		WorldSettings* settings)
	{
		RESOURCE::World* world = convertSettingsToResource(
			settings);

		if (!world)
		{
			return false;
		}

		const bool result = saveResourceToFile(
			fileName,
			world);
		delete world;

		return result;
	}
}
