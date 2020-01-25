#include "TileExit.h"

namespace Shared
{
	const TileDescription TileExit = TileDescription(
		TileCreation::Create<
			Game::TileExit,
			Editor::ExitTemplate,
			Resource::TileExit>(),

		L"Exit",
		L"Touch to finish the level",
		sf::Color::Color(50, 50, 200),
		sf::Color::Color(50, 50, 200)
	);
}

namespace Game
{
	GameTileBase* TileExit::Create(
		const Resource::Tile* const tile,
		const Resource::TileInstanceWrapper* const instanceWrapper,
		const TileIdentity identity)
	{
		return new TileExit(
			identity,
			sf::Vector2f(
				tile->content.x,
				tile->content.y
			),
			sf::Vector2f(
				tile->content.width,
				tile->content.height
		));
	}

	void TileExit::registerType(Environment* env)
	{
		CollidableTile::registerType(env);
		CollidableTile::registerCollisionType(
			env,
			CollisionType::NormalCollision
		);
		ExitableTile::registerType(env);
		StaticTile::registerType(env);
	}
}

namespace Editor
{
}
