#include "TransitivePortal.h"

const Shared::TileDescription Shared::TileTransitivePortal{
	Shared::TileCreation::Create<
		Game::TileTransitivePortal,
		Editor::TemplateTransitivePortal,
		Resource::TileTransitivePortal>(),
	L"Transitive Portal",
	L"Transitive Portal",
	sf::Color::Color(0xFF'00'FF'FF),
	sf::Color::Color(0xFF'00'FF'FF)
};

Game::GameTileBase* Game::TileTransitivePortal::Create(
	const Resource::Tile* const tile,
	const Resource::TileInstanceWrapper* const instanceWrapper,
	const Game::TileIdentity identity)
{
	return new TileTransitivePortal(
		identity,
		sf::Vector2f(
			tile->content.x,
			tile->content.y
		),
		sf::Vector2f(
			tile->content.width,
			tile->content.height
		),
		instanceWrapper->getInstance<Resource::TileTransitivePortal>()->content.world);
}
