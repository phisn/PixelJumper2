#pragma once

#include <SFML/System/Time.hpp>

#include <Client/source/game/tiletrait/GameTileBase.h>

#include <Client/source/game/Environment.h>

namespace Game
{
	class DynamicTile
		:
		public RegisterableType
	{
	public:
		void registerType(Environment* const environment) override
		{
			environment->registerTile<DynamicTile>(this);
		}

		virtual void processLogic() = 0;
	};
}
