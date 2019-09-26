#pragma once

#include <Client/source/shared/tiles/TileDescription.h>

#include <Client/source/game/tiletrait/CollidableTile.h>
#include <Client/source/game/tiletrait/ExitableTile.h>
#include <Client/source/game/tiletrait/StaticTile.h>

namespace Shared
{
	const extern TileDescription TileExit;
}

namespace Game
{
	class ExitTile
		:
		public CollidableTile,
		public ExitableTile,
		public StaticTile
	{
	public:
		static GameTileBase* Create(
			const Resource::Tile* const tile,
			const TileIdentity identity);

		ExitTile(
			const TileIdentity identity,
			const sf::Vector2f position,
			const sf::Vector2f size)
			:
			StaticTile(
				identity,
				Shared::TileExit.gameColor,
				position,
				size),
			CollidableTile(0, 0)
		{
		}

		void registerType(Environment* env) override;

		sf::Vector2f onCollision(
			const CollisionType type,
			const Collision collision) override
		{
			notifyCollisionEvent(&type, &collision);
			notifyExitEvent();

			return collision.target; // ignore collision
		}

		const sf::Vector2f getPosition() const override
		{
			return StaticTile::getPosition();
		}

		const sf::Vector2f getSize() const override
		{
			return StaticTile::getSize();
		}
	};
}
// TODO: imp
namespace Editor
{

}

namespace Resource
{

}
