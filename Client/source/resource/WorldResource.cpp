#include "WorldResource.h"

namespace Resource
{
	sf::Uint32 World::generateCheckSum()
	{
		sf::Uint32 result = (1 << 16) + (1 << 8) + 3;

		result += (decltype(result)) HeaderProperties.width;
		result -= (decltype(result)) ~HeaderProperties.height;
		result += (decltype(result)) HeaderProperties.tileCount;

		result *= (decltype(result)) HeaderIntro.worldID;
		result /= (decltype(result)) HeaderAuth.authorName.size();
		result -= (decltype(result)) ~HeaderAuth.mapName.size();

		for (Resource::Tile& tile : TileContainer)
		{ // TODO: check if not bad
			result *= (decltype(result)) tile.Header.id;
			result += (decltype(result)) ~tile.Header.height;
			result *= (decltype(result)) tile.Header.width;
			result += (decltype(result)) ~tile.Header.contentSize >> 2;
		}

		return result;
	}

	bool World::make(
		ReadPipe* const pipe)
	{
		if (pipe->getSize() < HEADER_SIZE)
		{
			return false;
		}

		////////// Header
		if (!readHeaderIntro(pipe))
		{
			return false;
		}

		if (!readHeaderAuth(pipe))
		{
			return false;
		}

		if (!readHeaderProperties(pipe))
		{
			return false;
		}

		////////// Conent
		TileContainer.clear();
		TileContainer.resize(HeaderProperties.tileCount);

		for (Tile& tile : TileContainer)
			if (!tile.make(pipe))
			{
				return false;
			}
		return true;
		//////////
		return HeaderProperties.tileCheckSum == generateCheckSum();
	}
	
	bool World::save(WritePipe* const pipe)
	{
		if (!setup())
		{
			return false;
		}

		////////// Header
		if (!pipe->writeValue(&HeaderIntro))
		{
			return false;
		}

		if (!pipe->writeString<AuthorNameSize>(&HeaderAuth.authorName) ||
			!pipe->writeString<MapNameSize>(&HeaderAuth.mapName))
		{
			return false;
		}

		if (!pipe->writeValue(&HeaderProperties))
		{
			return false;
		}

		////////// Content
		for (Tile& tile : TileContainer)
			if (!tile.save(pipe))
			{
				return false;
			}

		return true;
	}
	
	bool World::readHeaderIntro(ReadPipe * const pipe)
	{
		if (!pipe->readValue(&HeaderIntro))
		{
			return false;
		}

		return validateHeaderIntro();
	}
	
	bool World::readHeaderAuth(ReadPipe * const pipe)
	{
		if (!pipe->readString<AuthorNameSize>(&HeaderAuth.authorName))
		{
			return false;
		}

		if (!pipe->readString<MapNameSize>(&HeaderAuth.mapName))
		{
			return false;
		}

		return validateHeaderAuth();
	}
	
	bool World::readHeaderProperties(ReadPipe * const pipe)
	{
		if (!pipe->readValue(&HeaderProperties))
		{
			return false;
		}

		return validateHeaderProperties();
	}
	
	bool World::validateHeaderIntro() const
	{
		return HeaderIntro.magic == WORLD_MAGIC
			&& HeaderIntro.worldID != 0;
	}
	
	bool World::validateHeaderAuth() const
	{
		return HeaderAuth.authorName.size() > 0
			&& HeaderAuth.mapName.size() > 0;
	}

	// checksum not tested
	bool World::validateHeaderProperties() const
	{
		return HeaderProperties.width != 0
			&& HeaderProperties.height != 0
			&& HeaderProperties.tileCount != 0;
	}
}