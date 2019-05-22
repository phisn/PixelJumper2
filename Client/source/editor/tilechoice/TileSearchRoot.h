#pragma once

#include <Client/source/menu/MenuRootBase.h>

#include <Client/source/menu/MenuButton.h>
#include <Client/source/menu/MenuButtonMaterial.h>

#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/Manipulator.h>

#include <Client/source/editor/template/TileTemplateFactory.h>

namespace Editor
{
	class MagicTileButton
		:
		public Menu::Button<Menu::ButtonMaterial::DefaultRectangleStaticSize>
	{
	public:
		void onButtonPressed() override
		{
			TileChoiceInput* const input = Manipulator::GetCache()->tileChoice.writeInput();

			// ... add all templates

			Manipulator::GetCache()->tileChoice.notify();
		}
	};

	class TileSearchRoot
		:
		public Menu::MenuRootBase
	{
	public:
		bool build() override
		{
			addStaticChild(&button);

			size.addListener([this](
				const sf::Vector2f oldSize,
				const sf::Vector2f newSize)
			{
				button.size = newSize;
			});
		}
		
	private:
		MagicTileButton button;
		// textbox
	};
}
