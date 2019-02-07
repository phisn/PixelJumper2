#pragma once

#include <SFML/System.hpp>

namespace Game
{
	namespace CollisionEngine
	{
		struct CollisionContext
		{
			sf::Vector2f tilesSize;
			sf::Vector2f position;
			sf::Vector2f destination;
		};

		void SetCurrentContext(const CollisionContext);

		bool FindEnterCollision();
		bool FindLeaveCollision();
	}
}
