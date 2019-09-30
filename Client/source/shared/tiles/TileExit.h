#pragma once

#include <Client/source/editor/manipulator/Manipulator.h>
#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/Executor.h>
#include <Client/source/editor/manipulator/tasks/TilePlace.h>

#include <Client/source/editor/tile/TileTemplate.h>
#include <Client/source/editor/tilechoice/TileChoiceButton.h>

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
	class TileExit
		:
		public CollidableTile,
		public ExitableTile,
		public StaticTile
	{
	public:
		static GameTileBase* Create(
			const Resource::Tile* const tile,
			const TileIdentity identity);

		TileExit(
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

			collision.player->getProperties().position = collision.target;
			return { };
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

namespace Editor
{
	class TileExit
		:
		public TileBase
	{
	public:
		TileExit(const sf::Vector2f position)
			:
			TileBase(
				Shared::TileId::TileExit,
				Shared::TileExit.editorColor
			)
		{
			shape.setFillColor(getColor());
			setSize({ 1, 1 });
			setPosition(position);
		}

		Resource::TileBase* createContent(
			const Resource::VectorTileSize size,
			const Resource::VectorTilePosition position) const override;

		bool adopt(const Resource::TileBase* const tile) override
		{
			return true;
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
		sf::RectangleShape shape;
	};

	class ExitTemplate
		:
		public TileTemplate
	{
	public:
		ExitTemplate()
		{
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

			button.value.text = L"Exit";
		}

		TileBase* create(
			sf::Vector2f position) override
		{
			return new TileExit(position);
		}

		const std::wstring& getName() const override
		{
			return Shared::TileExit.name;
		}

		const std::wstring& getDescription() const override
		{
			return Shared::TileExit.info;
		}

		Shared::TileId getId() const override
		{
			return Shared::TileId::TileExit;
		}

		Menu::ElementBase* createRepresentation() override
		{
			return &button;
		}

		bool isSelected() const override
		{
			return button.isSelected();
		}

		void select() override
		{
			button.select();
		}

		void unselect() override
		{
			button.unselect();
		}

	private:
		TileChoiceButton<TileChoiceMaterial::Default> button;
	};
}

namespace Resource
{
#pragma pack(push, 1)

	class TileExit
		:
		public TileBase
	{
	public:
		TileExit()
			:
			TileBase(
				Shared::TileId::TileExit,
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
		} Content = { };
	};

#pragma pack(pop)
}
