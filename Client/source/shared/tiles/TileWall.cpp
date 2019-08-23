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
		Resource::Tile* const tile,
		const int identity)
	{
		Resource::WallTile* const resource = (Resource::WallTile*) tile->Content;

		if (tile->Header.contentSize != sizeof(Resource::WallTile::Content))
		{
			return NULL;
		}

		return new WallTile(
			identity,

			resource->Content.density,
			resource->Content.friction,

			sf::Vector2f(
				tile->Header.x,
				tile->Header.y
			),
			sf::Vector2f(
				tile->Header.width,
				tile->Header.height
			)
		);
	}

	void WallTile::registerType(Environment* const env)
	{
		CollidableTile::registerCollisionType(
			env,
			CollisionType::NormalCollision
		);
		StaticTile::registerType(env);
	}
	
	sf::Vector2f WallTile::onCollision(const CollisionType type, const Collision collision)
	{
		CollidableTile::onCollision(type, collision);
		collision.player->state.position = collision.info.position;

		// predefine 0
		sf::Vector2f movement = {};
		sf::Vector2f remainMove = {};

		// set opposite of collision
		if (collision.info.isHorizontal())
		{
			movement.y = collision.player->state.readProperties()->movement.y;
			remainMove.y = (collision.target.y - collision.info.position.y);
		}
		else
		{
			movement.x = collision.player->state.readProperties()->movement.x;
			remainMove.x = (collision.target.x - collision.info.position.x);
		}

		collision.player->state.movement = movement;
		return remainMove;
	}
}

namespace Editor
{
	Menu::ElementBase* WallTemplate::createRepresentation()
	{
		static TileChoiceButton<TileChoiceMaterial::Default> button;
		static bool initialized = false;

		if (!initialized)
		{
			initialized = true;

			button.sizePreferred = { 300, 100 };
			button.buttonSelectedEvent.addListener(
				[this]()
				{
					Manipulator::GetCache()->tileChoice.writeInput()->selection = this;

					if (!Manipulator::GetCache()->tileChoice.notify())
					{
						Log::Error(L"Failed to select template");
					}
				});
			button.buttonPressedEvent.addListener(
				[this]()
				{
					if (!Manipulator::GetExecutor()->execute<TilePlace>())
					{
						Log::Error(L"Failed to place tiles");
					}
				});

			button.label.text = L"Wall";
		}

		return &button;
	}
	
	Resource::TileBase* TileWall::createContent(
		const Resource::VectorTileSize size, 
		const Resource::VectorTilePosition position) const
	{
		Resource::WallTile* const tile = new Resource::WallTile();

		tile->Content.density = density; // TODO: Make Variable
		tile->Content.friction = friction;

		return tile;
	}
	
	bool TileWall::adopt(
		const Resource::TileBase* const tile)
	{
		Resource::WallTile* const wt = (Resource::WallTile*) tile;
		+
		density = wt->Content.density;
		friction = wt->Content.friction;

		return true; // ignore
	}
}
