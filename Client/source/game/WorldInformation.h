#pragma once

#include <Client/source/resource/WorldResource.h>

#include <string>

namespace Game
{
	struct WorldInformation
	{
		std::wstring authorName;
		std::wstring mapName;

		Resource::WorldId worldId;
		sf::Vector2i size;
	};
}
