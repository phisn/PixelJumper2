#pragma once

#include <SFML/System/Vector2.hpp>

namespace Game
{
	struct RegisterableType
	{
		virtual void registerType(Environment* const) = 0;
	};

	class Environment;
	class GameTileBase : public RegisterableType
	{
	public:
		// should register itself into env
		virtual void registerType(Environment* const) = 0;

		virtual const sf::Vector2f getPosition() const = 0;
		virtual const sf::Vector2f getSize() const = 0;
	};
}
