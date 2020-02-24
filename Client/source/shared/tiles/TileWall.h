#pragma once

#include <Client/source/editor/manipulator/Manipulator.h>
#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/Executor.h>
#include <Client/source/editor/manipulator/tasks/TilePlace.h>

#include <Client/source/editor/tile/TileTemplate.h>
#include <Client/source/editor/tilechoice/TileChoiceButton.h>

#include <Client/source/game/GameWorld.h>

#include <Client/source/game/component/SolidCollisionComponent.h>
#include <Client/source/game/GameTileBase.h>
#include <Client/source/game/tiletrait/StaticTile.h>

#include <Client/source/shared/tiles/TileDescription.h>
#include <Client/source/shared/ValueDefinitions.h>

namespace Shared
{
	const extern TileDescription TileWall;
}

namespace Game
{
	class WallTile
		:
		public GameTileBase,
		public StaticTile
	{
	public:
		static GameTileBase* Create(
			const Resource::Tile* const tile,
			const Resource::TileInstanceWrapper* const instanceWrapper,
			const TileIdentity identity);

		WallTile(
			const TileIdentity identity,
			const TileContent content,
			const SolidCollisionContent collisionContent)
			:
			GameTileBase(identity, content),
			StaticTile(
				Shared::TileWall.gameColor, 
				GameTileBase::content),
			collisionComponent(
				GameTileBase::content,
				collisionContent)
		{
		}

		void registerComponents(Environment* const environment) override
		{
			collisionComponent.registerComponent(environment);
		}

	private:
		SolidCollisionComponent collisionComponent;
	};
}

namespace Editor
{
	class TileWall
		:
		public TileBase
	{
	public:
		TileWall(const sf::Vector2f position)
			:
			TileBase(
				Shared::TileID::TileWall, 
				Shared::TileWall.editorColor)
		{
			shape.setFillColor(getColor());
			setSize({ 1, 1 });
			setPosition(position);

			content.density = Shared::ValueDefinition::tile_density;
			content.inputForceAddition = Shared::ValueDefinition::tile_input;
			content.friction = Shared::ValueDefinition::tile_friction;
		}

		void assignInstance(const Resource::TileInstanceWrapper* const instanceWrapper) const override;
		bool adopt(const Resource::TileInstanceWrapper* const instanceWrapper) override;

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
		Game::SolidCollisionContent content;
		sf::RectangleShape shape;
	};

	class WallTemplate
		:
		public TileTemplate
	{
	public:
		WallTemplate()
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

			button.value.text = L"Wall";
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

		Shared::TileID getId() const override
		{
			return Shared::TileID::TileWall;
		}

		Menu::ElementBase* createRepresentation() override;

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

	class WallTile
		:
		public ResourceBase
	{
	public:
		Game::SolidCollisionContent content;

		WallTile()
		{
			content.density = Shared::ValueDefinition::tile_density;
			content.friction = Shared::ValueDefinition::tile_friction;
			content.inputForceAddition = Shared::ValueDefinition::tile_input;
		}

		bool make(ReadPipe* const pipe) override
		{
			return pipe->readValue(&content);
		}

		bool save(WritePipe* const pipe) override
		{
			return pipe->writeValue(&content);
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
