#pragma once

#include <Client/source/game/tiles/TileColor.h>

#include <Client/source/editor/TileProperty.h>
#include <Client/source/editor/TileTemplate.h>


namespace Editor
{
	class WallProperties
	{
	public:
		std::vector<TileProperty> properties;
	};

	class WallTemplate
		:
		public TileTemplate
	{
	public:
		WallTemplate()
			:
			TileTemplate(TILE_WALL_COLOR)
		{
		}

		bool onOpenContextMenu() override
		{
			return false;
		}

		void makeProperties(WallProperties* wall)
		{
			wall->properties.clear();

			wall->properties.emplace_back();
			TileProperty* property = &wall->properties.back();

			property->type = Type::Bool;
			property->value = new bool(0);
		}
	};
}
