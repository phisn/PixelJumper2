#pragma once

#include <Client/source/menu/element/ButtonBase.h>

#include <Client/source/editor/manipulator/Manipulator.h>
#include <Client/source/editor/manipulator/cache/CacheManager.h>
#include <Client/source/editor/manipulator/cache/Cache.h>

#include <Client/source/editor/template/TileTemplate.h>

#include <functional>

namespace Editor
{
	class TileMenuButton
		:
		public MENU::ButtonBase
	{
	public:
		struct Style
		{
			TileTemplate* tile;

			sf::Color 
				fillColor, 
				clickColor;

			MENU::ButtonBase::Style makeStyle() const
			{
				MENU::ButtonBase::Style style = { };
				
				style.enter_fillColor = fillColor;
				style.enter_outlineColor = sf::Color::Color(150, 150, 150);
				style.enter_outlineThickness = 0.2f;


				style.click_fillColor = clickColor;
				style.click_outlineColor = { };
				style.click_outlineThickness = 0.0f;


				style.default_fillColor = fillColor;
				style.click_outlineColor = { };
				style.click_outlineThickness = 0.0f;

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
	};
}
