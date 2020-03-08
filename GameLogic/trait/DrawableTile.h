#pragma once

#include <SFML/Graphics.hpp>

namespace Game
{
	// not possible with static tiles
	class DrawableTile
	{
	public:
		virtual void onDraw(sf::RenderTarget* const target) = 0;
	};
}
