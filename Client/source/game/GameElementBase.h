#pragma once

#include <Client/source/game/Environment.h>

namespace Game
{
	typedef int TileIdentity;

	class GameElementBase
	{
	public:
		GameElementBase(const TileIdentity identity)
			:
			identity(identity)
		{
		}

		TileIdentity getIdentity() const
		{
			return identity;
		}

		virtual void registerComponents(Environment* const environment) = 0;

	private:
		const TileIdentity identity;
	};
}
