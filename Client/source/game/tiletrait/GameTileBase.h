#pragma once

#include <SFML/System/Vector2.hpp>

namespace Game
{
	class Environment;
	struct RegisterableType
	{
		virtual void registerType(Environment* const) = 0;
	};

	class GameTileBase : public RegisterableType
	{
	public:
		GameTileBase(const int identity)
		{
		}

		// should register itself into env
		virtual void registerType(Environment* const) = 0;

		virtual const sf::Vector2f getPosition() const = 0;
		virtual const sf::Vector2f getSize() const = 0;

		int getIdentity() const
		{
			return identity;
		}

	private:
		const int identity;
	};
}
