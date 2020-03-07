#pragma once

#include "Common.h"
#include "Environment.h"

namespace Game
{
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
