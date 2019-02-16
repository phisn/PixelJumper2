#pragma once

#include <SFML/System/Time.hpp>

namespace Game
{
	class DynamicTile
	{
	public:
		virtual void onLogic(const sf::Time time) = 0;
	};
}
