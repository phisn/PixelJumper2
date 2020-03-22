#pragma once

#include <Client/source/editor/manipulator/Manipulator.h>
#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/Executor.h>
#include <Client/source/editor/manipulator/tasks/TilePlace.h>

#include <Client/source/editor/tile/TileTemplate.h>
#include <Client/source/editor/tilechoice/TileChoiceButton.h>

#include <Client/source/game/GameWorld.h>

#include <Client/source/game/component/DynamicWorldTransitionComponent.h>
#include <Client/source/game/component/EmptyCollisionComponent.h>
#include <Client/source/game/GameTileBase.h>
#include <Client/source/game/tiletrait/StaticTile.h>

#include <Client/source/shared/tiles/TileDescription.h>
#include <Client/source/shared/ValueDefinitions.h>

namespace Shared
{
	const extern TileDescription TileTransitivePortal;
}

namespace Game
{
	class TileTransitivePortal
		:
		public GameTileBase,
		public StaticTile
	{
	public:
		static GameTileBase* Create(
			const Resource::Tile* const tile,
			const Resource::TileInstanceWrapper* const instanceWrapper,
			const TileIdentity identity);

		TileTransitivePortal(
			const TileIdentity identity,
			const TileContent content,
			const DynamicWorldTransitionContent transitionContent)
			:
			GameTileBase(
				identity,
				content),
			StaticTile(
				Shared::TileTransitivePortal.gameColor, 
				content),
			collisionComponent(content),
			transitionComponent(
				transitionContent,
				content)
		{
		}

		void registerComponents(Environment* const environment) override
		{
			collisionComponent.registerComponent(
				environment,
				CollisionType{
					true,
					false,
					true
				});
			transitionComponent.registerComponent(environment);
		}

	private:
		DynamicWorldTransitionComponent transitionComponent;
		EmptyCollisionComponent collisionComponent;
	};
}

namespace Editor
{
	class TileTransitivePortal
		:
		public TileBase
	{
	public:
		TileTransitivePortal(const sf::Vector2f position)
			:
			TileBase(
				Shared::TileID::TileTransitivePortal,
				Shared::TileTransitivePortal.editorColor)
		{
			shape.setFillColor(getColor());
			setSize({ 1, 1 });
			setPosition(position);
		}

		void assignInstance(const Resource::TileInstanceWrapper* const instanceWrapper) const override
		{
		}

		void adoptInWorld(Resource::World* const world) override
		{
			if (std::find(
					world->targets.begin(),
					world->targets.end(),
					content.targetWorld) 
				== world->targets.end())
				world->targets.push_back(content.targetWorld);
		}

		bool adopt(const Resource::TileInstanceWrapper* const instanceWrapper) override
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
		Game::DynamicWorldTransitionContent content;
		sf::RectangleShape shape;
	};

	class TemplateTransitivePortal
		:
		public TileTemplate
	{
	public:
		TemplateTransitivePortal()
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

			button.value.text = L"Transitive Portal";
		}

		// onOpenContext ...

		// ex. can be saved and shon in context menu (portal binding)
		TileBase* create(
			sf::Vector2f position) override
		{
			return new TileTransitivePortal(position);
		}

		const std::wstring& getName() const override
		{
			return Shared::TileTransitivePortal.name;
		}

		const std::wstring& getDescription() const override
		{
			return Shared::TileTransitivePortal.info;
		}

		Shared::TileID getId() const override
		{
			return Shared::TileID::TileTransitivePortal;
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

	class TileTransitivePortal
		:
		public ResourceBase
	{
	public:
		Game::DynamicWorldTransitionContent content;

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
