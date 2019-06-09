#pragma once

#include <Client/source/menu/MenuRootBase.h>

#include <Client/source/menu/MenuButton.h>
#include <Client/source/menu/MenuButtonMaterial.h>

#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/Manipulator.h>

#include <Client/source/editor/template/TileTemplateFactory.h>
#include <Client/source/editor/tilechoice/TileSearchWorker.h>

namespace Editor
{
	class MagicTileButton
		:
		public Menu::Button<Menu::ButtonMaterial::DefaultRectangle>
	{
	public:
		void onButtonPressed() override
		{
			TileSearchInput* const input = Manipulator::GetCache()->tileSearch.writeInput();

			Manipulator::GetCache()->tileSearch.notify();
		}
	};

	class TileSearchRoot
		:
		public Menu::MenuRootBase
	{
	public:
		bool build() override
		{
			addChild(&button);

			size.addListener([this](
				const sf::Vector2f oldSize,
				const sf::Vector2f newSize)
			{
				button.space = newSize;
				button.sizePreferred = newSize;
			});
			innerOffset = { 5.f, 5.f, 5.f, 5.f };

			Manipulator::GetCache()->tileSearch.addWorker(
				std::ref(worker), 0
			);
			
			return true;
		}


	private:
		MagicTileButton button;
		TileSearchWorker worker;
	};
}