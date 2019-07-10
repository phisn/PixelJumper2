#pragma once

#include <Client/source/editor/tile/EditorTileBase.h>
#include <Client/source/editor/selector/SelectorView.h>

#include <Client/source/game/tile/GameWallTile.h>

namespace Editor
{
	namespace Tile
	{
		class Wall
			:
			public TileBase
		{
		public:
			Wall(
				const sf::Vector2f position)
				:
				TileBase(Game::TileId::Wall, Game::WallTile::COLOR)
			{
				shape.setFillColor(getColor());
				setSize({ 1, 1 });
				setPosition(position);
			}

			Resource::TileBase* createContent(
				const Resource::VectorTileSize size,
				const Resource::VectorTilePosition position) const override 
			{
				Resource::WallTile* const tile = new Resource::WallTile();
				
				tile->Content.density = density; // TODO: Make Variable
				tile->Content.friction = friction;

				return tile;
			}

			bool adopt(const Resource::TileBase* const tile) override
			{
				Resource::WallTile* const wt = (Resource::WallTile*) tile;

				density = wt->Content.density;
				friction = wt->Content.friction;

				return true; // ignore
			}

			virtual void setPosition(const sf::Vector2f position)
			{
				TileBase::setPosition(position);
				shape.setPosition(position * SelectorView::GridRectSize);
			}

			virtual void setSize(const sf::Vector2f size)
			{
				TileBase::setSize(size);
				shape.setSize(size * SelectorView::GridRectSize);
			}

			void draw(sf::RenderTarget* const target) const override
			{
				target->draw(shape);
			}

		private:
			float density = 2.f;
			float friction = 1.f;

			sf::RectangleShape shape;
		};
	}
}