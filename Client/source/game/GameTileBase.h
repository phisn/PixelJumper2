#pragma once

#include <Client/source/game/GameElementBase.h>
#include <Client/source/game/tiletrait/TileContent.h>

namespace Game
{
	class GameTileBase
		:
		public GameElementBase
	{
	public:
		GameTileBase(
			const int identity,
			const TileContent content)
			:
			GameElementBase(identity),
			content(content)
		{
		}

		const sf::Vector2f getPosition() const
		{
			return content.position;
		}

		const sf::Vector2f getSize() const
		{
			return content.size;
		}

	protected:
		const TileContent content;
	};
}
