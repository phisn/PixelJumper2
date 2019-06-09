#pragma once

#include <Client/source/game/tile/GameWallTile.h>

#include <Client/source/editor/template/TileTemplate.h>
#include <Client/source/editor/template/TileTemplateFactory.h>

#include <Client/source/editor/tile/EditorWall.h>

#include <Client/source/editor/manipulator/Manipulator.h>
#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/Executor.h>
#include <Client/source/editor/manipulator/tasks/TilePlace.h>

#include <Client/source/menu/MenuButtonMaterial.h>
#include <Client/source/menu/MenuButtonWithLabel.h>
#include <Client/source/editor/tilechoice/TileChoiceButton.h>

#include <iostream>

namespace Editor
{
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
			return new Tile::Wall(position);
		}

		const std::wstring& getName() const override
		{
			return name;
		}
		
		const std::wstring& getDescription() const override
		{
			return description;
		}

		Game::TileId getId() const override
		{
			return Game::TileId::Wall;
		}

		// get?
		Menu::ElementBase* createRepresentation() override
		{
			//static Menu::ButtonWithLabel<Menu::ButtonMaterial::DefaultStaticRectangle> button;
			static TileChoiceButton button;

			button.sizePreferred = { 300, 100 };
			button.buttonPressedEvent.addListener(
				[this]()
				{
					Manipulator::GetCache()->tileChoice.writeInput()->selection = this;

					if (Manipulator::GetCache()->tileChoice.notify() && !Manipulator::GetExecutor()->execute<TilePlace>())
					{
						Log::Error(L"Failed to place tiles");
					}
				});

			return &button;
		}

	private:
		std::wstring name = L"Wall";
		std::wstring description = L"A solid tile";
	};
}
