#pragma once

#include <Client/source/game/Environment.h>
#include <Client/source/game/tiletrait/GameElementBase.h>

namespace Game
{
	class GameTileBase
		:
		public GameElementBase
	{
	public:
		virtual void registerType(Environment* const env) override
		{
			env->registerTile<GameTileBase>(this);
		}

		// should not be used and is only
		// here to symplify virtual inheritance
		GameTileBase()
			:
			GameTileBase(0, { }, { })
		{
			assert(true);
		}

		GameTileBase(
			const int identity,
			const sf::Vector2f position,
			const sf::Vector2f size)
			:
			GameElementBase(identity),
			position(position),
			size(size)
		{
		}

		const sf::Vector2f getPosition() const
		{
			return position;
		}

		const sf::Vector2f getSize() const
		{
			return size;
		}

	protected:
		const sf::Vector2f position;
		const sf::Vector2f size;
	};
}
