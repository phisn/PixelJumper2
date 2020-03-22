#include "TileWall.h"

namespace Shared
{
	const TileDescription TileWall = TileDescription(
		TileCreation::Create<
			Game::WallTile, 
			Editor::WallTemplate, 
			Resource::WallTile>(),

		L"Wall",
		L"Solid Wall",
		sf::Color::Color(200, 200, 200),
		sf::Color::Color(200, 200, 200)
	);
}

namespace Game
{
	GameTileBase* WallTile::Create(
		const Resource::Tile* const tile,
		const Resource::TileInstanceWrapper* const instanceWrapper,
		const TileIdentity identity)
	{
		return new WallTile(
			identity,
			{
				sf::Vector2f(
					tile->content.x,
					tile->content.y),
				sf::Vector2f(
					tile->content.width,
					tile->content.height)
			},
			instanceWrapper->getInstance<Resource::WallTile>()->content);
	}
}

namespace Editor
{
	Menu::ElementBase* WallTemplate::createRepresentation()
	{
		return &button;
	}
	
	void TileWall::assignInstance(const Resource::TileInstanceWrapper* const instanceWrapper) const
	{
		instanceWrapper->getInstance<Resource::WallTile>()->content = content;
	}

	bool TileWall::adopt(const Resource::TileInstanceWrapper* const instanceWrapper)
	{
		content = instanceWrapper->getInstance<Resource::WallTile>()->content;
		return true;
	}
}
