#pragma once

#include <Client/source/game/Environment.h>
#include <Client/source/game/tiletrait/GameTileBase.h>

namespace Game
{
	// not possible with static tiles
	class DrawableTile 
		: 
		public RegisterableType
	{
	public:
		void registerType(Environment* const env)
		{
			env->registerTile<DrawableTile>(this);
		}

		virtual void onDraw(sf::RenderTarget* const target) = 0;
	};
}
