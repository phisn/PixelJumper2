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
		public StaticTile,
		public GameTileBase
	{
	public:
		static GameTileBase* Create(
			const Resource::Tile* const tile,
			const Resource::TileInstanceWrapper* const instanceWrapper,
			const TileIdentity identity);

		TileExit(
			const TileIdentity identity,
			const TileContent content)
			:
			GameTileBase(
				identity,
				content),
			StaticTile(
				Shared::TileExit.gameColor,
				content),
			CollidableTile(content)
		{
		}

		sf::Vector2f onCollision(
			const CollisionType type,
			const Collision& collision) override
		{
			notifyCollisionEvent(&type, &collision);
			notifyExitEvent();

			collision.player->getProperties().position = collision.target;
			return { };
		}

		void registerComponents(Environment* const environment) override
		{
			environment->registerCollisionType(
				CollisionType::NormalCollision, 
				this);

			environment->registerTile<CollidableTile>(this);
			environment->registerTile<ExitableTile>(this);
			environment->registerTile<StaticTile>(this);
			environment->registerTile<GameTileBase>(this);
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
				Shared::TileID::TileExit,
				Shared::TileExit.editorColor
			)
		{
			shape.setFillColor(getColor());
			setSize({ 1, 1 });
			setPosition(position);
		}


		bool adopt(const Resource::TileInstanceWrapper* const instanceWrapper)
		{
			return true;
		}

		void assignInstance(const Resource::TileInstanceWrapper* const instanceWrapper) const
		{
		}

		void adoptInWorld(Resource::World* const world) override
		{
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

		Shared::TileID getId() const override
		{
			return Shared::TileID::TileExit;
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
		public ResourceBase
	{
	public:
		TileExit()
		{
		}

		bool make(ReadPipe* const pipe) override
		{
			return true;
		}

		bool save(WritePipe* const pipe) override
		{
			return true;
		}

		bool setup() override
		{
			return true;
		}

		bool validate() override
		{
			return true;
		}
	};

#pragma pack(pop)
}
