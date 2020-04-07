#pragma once

#include <Client/source/menu/MenuBackground.h>
#include <Client/source/menu/MenuRootBase.h>
#include <Client/source/menu/MenuTextBoxWithInfo.h>

#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/Manipulator.h>

#include <Client/source/editor/tile/TileTemplateFactory.h>
#include <Client/source/editor/tilechoice/TileSearchWorker.h>

#include <Client/source/shared/tiles/TileDescription.h>

namespace Editor
{
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
					updateTileSearch();
				}
			);
		}

		void updateTileSearch()
		{
			TileSearchInput* const input = Manipulator::GetCache()->tileSearch.writeInput();

			input->search = text;

			if (!Manipulator::GetCache()->tileSearch.notify())
			{
				Log::Error(L"Failed to notify TileSearch");
			}
		}
	};

	class TileSearchRoot
		:
		public Menu::MenuRootBase
	{
	public:
		bool build() override
		{
			addChild(&background);
			addChild(&textBox);

			size.addListener([this](
				const sf::Vector2f oldSize,
				const sf::Vector2f newSize)
				{
					background.sizePreferred = newSize;
					
					textBox.space = newSize;
					textBox.sizePreferred = newSize;
				});

			background.color = sf::Color::Color(20, 20, 20);
			textBox.outerOffset = { 5.f, 5.f, 5.f, 5.f };

			Manipulator::GetCache()->tileSearch.addWorker(
				std::ref(worker), 0
			);

			textBox.updateTileSearch();

			return true;
		}

		void onDraw(sf::RenderTarget* const target) const override
		{
			Menu::MenuRootBase::onDraw(target);
		}

	private:
		TileSearchTextBox textBox;
		TileSearchWorker worker;

		Menu::Background background;
	};
}
