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

		void addLocalPlayer(
			LocalPlayer* localPlayer)
		{
			localPlayers.push_back(localPlayer);
		}

		void addRemotePlayer(
			RemotePlayer* remotePlayer)
		{
			remotePlayers.push_back(remotePlayer);
		}

		void clearPlayers()
		{
			localPlayers.clear();
			remotePlayers.clear();
		}
	private:
		bool initialize()
		{
			if (localPlayers.size() == 0 || localPlayers.size() > 4)
			{
				return false;
			}

			for (int i = 0; i < localPlayers.size(); ++i)
			{
				localPlayers[i]->getView().adjustView(
					i, localPlayers.size());
			}

			return true;
		}

		std::vector<LocalPlayer*> localPlayers;
		std::vector<RemotePlayer*> remotePlayers;

		Map* map;
	};
}
