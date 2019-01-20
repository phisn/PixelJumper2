#pragma once

#include <Client/source/menu/element/ButtonBase.h>

#include <Client/source/editor/manipulator/Manipulator.h>
#include <Client/source/editor/manipulator/cache/CacheManager.h>
#include <Client/source/editor/manipulator/cache/Cache.h>

#include <Client/source/editor/template/TileTemplate.h>

#include <functional>


// TODO: remove
#include <Client/source/editor/manipulator/Executor.h>
#include <Client/source/editor/manipulator/Manipulator.h>
#include <Client/source/editor/manipulator/tasks/TilePlace.h>

#include <iostream>

namespace Editor
{
	class TileMenuButton
		:
		public MENU::ButtonBase
	{
	public:
		struct Style
		{
			const sf::Color enterOffset = sf::Color(20, 20, 20);
			const sf::Color clickOffset = sf::Color(200, 200, 200); // negative

			TileTemplate* tile;

			MENU::ButtonBase::Style makeStyle() const
			{
				MENU::ButtonBase::Style style = { };
				
				style.enter_fillColor = tile->color + enterOffset;
				style.enter_outlineColor = sf::Color::Color(150, 150, 150);
				style.enter_outlineThickness = 0.2f;

				style.click_fillColor = tile->color * clickOffset;
				style.click_outlineColor = sf::Color::Color(200, 200, 200);
				style.click_outlineThickness = 2.f;

				style.default_fillColor = tile->color;
				style.default_outlineColor = { };
				style.default_outlineThickness = 0.0f;

				return style;
			}
		};

		TileMenuButton(
			const Style style)
			:
			MENU::ButtonBase( style.makeStyle() ),
			tile( style.tile ),
			label( style.tile->name )
		{
		}

		void onLogic(
			const sf::Time time) override { }

		void unselect()
		{
			selected = false;
			setDefaultStyle();
		}

		TileTemplate* getTemplate() const
		{
			return tile;
		}

	private:
		bool selected = false;

		std::wstring label;
		TileTemplate* tile;

		void onMouseEnter() override
		{
			if (!selected)
			{
				MENU::ButtonBase::onMouseEnter();
			}
		}

		void onMouseLeave() override
		{
			if (!selected)
			{
				MENU::ButtonBase::onMouseLeave();
			}
		}

		void onMouseClick() override
		{
			if (selected)
			{
				::std::cout << "Place" << std::endl;
				Manipulator::getExecutor()->execute<TilePlace>();

				return;
			}

			selected = true;

			Manipulator::getCache()->writeInput()->tile.tile = tile;
			Manipulator::getCacheManager()->notify(
				Cache::Sector::Tile
			); // container as component
		} 

		void onMouseClickEnd() override
		{
			if (!getIsInside())
			{
				MENU::ButtonBase::onMouseClickEnd();
			}
		}

		sf::Text text;
	};
}
