#pragma once
/*
#include <Client/source/game/tiles/TileBase.h>

#include <Client/source/resource/WorldResource.h>

#include <SFML/Main.hpp>

#include <fstream>
#include <string>
#include <vector>

namespace Game
{
	struct WorldSettings
	{
		~WorldSettings()
		{
			for (Tile::Base* tile : tiles)
			{
				delete tile;
			}
		}

		std::vector<Tile::Base*> tiles;

		sf::Vector2u size;
		std::wstring author, name;
	};

	namespace WorldManager
	{
		WorldSettings* loadSettingsFromFile(
			const std::wstring fileName);
		bool saveSettingsToFile(
			const std::wstring fileName,
			WorldSettings* settings);

		RESOURCE::_N_World* loadResourceFromFile(
			const std::wstring fileName);
		bool saveResourceToFile(
			const std::wstring fileName,
			RESOURCE::_N_World* world);

		RESOURCE::_N_World* convertSettingsToResource(
			WorldSettings* settings);
		WorldSettings* convertResourceToSettings(
			RESOURCE::_N_World* world);
	}
}
*/