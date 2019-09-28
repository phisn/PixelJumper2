#pragma once

#include <Client/source/editor/manipulator/Manipulator.h>
#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/Executor.h>
#include <Client/source/editor/manipulator/tasks/TilePlace.h>

#include <Client/source/editor/tile/TileTemplate.h>
#include <Client/source/editor/tilechoice/TileChoiceButton.h>

#include <Client/source/game/GameWorld.h>

#include <Client/source/game/tiletrait/CollidableTile.h>
#include <Client/source/game/tiletrait/StaticTile.h>

#include <Client/source/resource/ResourceTileContentBase.h>

#include <Client/source/shared/tiles/TileDescription.h>


namespace Shared
{
	const extern TileDescription TileWall;
}

namespace Game
{
	class WallTile
		:
		public CollidableTile,
		public StaticTile
	{
	public:
		static GameTileBase* Create(
			const Resource::Tile* const tile, 
			const TileIdentity identity);

		WallTile(
			const TileIdentity identity,
			const float density,
			const float drag,
			const sf::Vector2f position,
			const sf::Vector2f size)
			:
			StaticTile(
				identity,
				Shared::TileWall.gameColor,
				position,
				size),
			CollidableTile(density, drag) // 10.f, 0.01f
		{
		}

		void setDND(float d1, float d2)
		{
			density = d1;
			friction = d2;
		}

		void registerType(Environment* const env) override;

		virtual sf::Vector2f onCollision(
			const CollisionType type,
			const Collision collision) override;

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

namespace Editor
{
	class TileWall
		:
		public TileBase
	{
	public:
		TileWall(
			const sf::Vector2f position)
			:
			TileBase(
				Game::TileId::TileWall, 
				Shared::TileWall.editorColor)
		{
			shape.setFillColor(getColor());
			setSize({ 1, 1 });
			setPosition(position);
		}

		Resource::TileBase* createContent(
			const Resource::VectorTileSize size,
			const Resource::VectorTilePosition position) const override;

		bool adopt(const Resource::TileBase* const tile) override;

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
		float density = 0.04f;
		float friction = 1.f;

		sf::RectangleShape shape;
	};

	class WallTemplate
		:
		public TileTemplate
	{
	public:
		WallTemplate()
		{
		}

		// onOpenContext ...

		// ex. can be saved and shon in context menu (portal binding)
		TileBase* create(
			sf::Vector2f position) override
		{
			return new TileWall(position);
		}

		const std::wstring& getName() const override
		{
			return Shared::TileWall.name;
		}

		const std::wstring& getDescription() const override
		{
			return Shared::TileWall.info;
		}

		Game::TileId getId() const override
		{
			return Game::TileId::TileWall;
		}

		Menu::ElementBase* createRepresentation() override;
	};
}


namespace Resource
{
#pragma pack(push, 1)

	class WallTile
		:
		public TileBase
	{
	public:
		WallTile()
			:
			TileBase(
				Shared::TileId::TileWall,
				sizeof(Content)
			)
		{
		}

		bool make(ReadPipe* const pipe) override
		{
			return pipe->readValue(&Content);
		}

		bool save(WritePipe* const pipe) override
		{
			return pipe->writeValue(&Content);
		}

		bool setup() override
		{
			return true;
		}

		struct
		{
			float density;
			float friction;

		} Content = { };
	};

#pragma pack(pop)
}
