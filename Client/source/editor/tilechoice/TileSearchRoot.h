#pragma once

#include <Client/source/menu/MenuRootBase.h>
#include <Client/source/menu/MenuTextBoxWithInfo.h>

#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/Manipulator.h>

#include <Client/source/editor/tile/TileTemplateFactory.h>
#include <Client/source/editor/tilechoice/TileSearchWorker.h>

namespace Editor
{
	/*class MagicTileButton
		:
		public Menu::Button<Menu::ButtonMaterial::DefaultStaticRectangle>
	{
	public:
		void onButtonPressed() override
		{
			TileSearchInput* const input = Manipulator::GetCache()->tileSearch.writeInput();

			Manipulator::GetCache()->tileSearch.notify();
		}
	};*/

	class TileSearchTextBox
		:
		public Menu::TextBoxWithInfo
	{
	public:
		TileSearchTextBox()
		{
			text.addListener(
				[this](const std::wstring& oldString,
					   const std::wstring& newString)
				{
					TileSearchInput* const input = Manipulator::GetCache()->tileSearch.writeInput();

				}
			);
		}
	};

	class TileSearchRoot
		:
		public Menu::MenuRootBase
	{
	public:
		bool build() override
		{
			addChild(&textBox);

			size.addListener([this](
				const sf::Vector2f oldSize,
				const sf::Vector2f newSize)
			{
				textBox.space = newSize;
				textBox.sizePreferred = newSize;
			});
			innerOffset = { 5.f, 5.f, 5.f, 5.f };

			Manipulator::GetCache()->tileSearch.addWorker(
				std::ref(worker), 0
			);
			
			return true;
		}


	private:
		TileSearchTextBox textBox;
		TileSearchWorker worker;
	};
}
