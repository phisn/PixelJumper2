#pragma once

#include "LocalPlayer.h"
#include "Map.h"
#include "MapLoader.h"
#include "RemotePlayer.h"

#ifndef PJ
#define PJ ::PixelJumper
#endif

#include <string>
#include <vector>

namespace Scene
{
	class PixelJumper;
}

namespace PixelJumper
{
	class Context
	{
		// Direct access on content
		friend ::Scene::PixelJumper;
	public:
		void loadMapByFile(
			std::wstring fileName)
		{


		}


	private:
		std::vector<Player*> viewedPlayers;

		std::vector<LocalPlayer*> localPlayers;
		std::vector<RemotePlayer*> remotePlayers;

		Map* map;
	};
}
