#pragma once

#include <SFML/System/Vector2.hpp>

namespace Game
{
	typedef int TileIdentity;

	class Environment;
	struct RegisterableType
	{
		virtual void registerType(Environment* const) = 0;
	};

	class GameElementBase 
		:
		public RegisterableType
	{
	public:
		GameElementBase(const TileIdentity identity)
			:
			identity(identity)
		{
		}

		// should register itself into env
		virtual void registerType(Environment* const) = 0;

		TileIdentity getIdentity() const
		{
			return identity;
		}

	private:
		const TileIdentity identity;
	};
}
