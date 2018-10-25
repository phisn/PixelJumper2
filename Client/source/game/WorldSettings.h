#pragma once

#include <SFML/Main.hpp>

#define WORLD_MAGIC 0xbb'22'cc'33

namespace Game
{
	struct WorldSettings
	{
		struct
		{
			sf::Uint32 magic = WORLD_MAGIC;
			sf::Uint64 worldId;

			float width;
			float height;

			sf::Int32 tileCount;
			sf::Int32 contentSize;

		} header;

		struct
		{


		} content;
	};
}
