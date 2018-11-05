#pragma once

#include <Client/source/game/tiles/TileColor.h>

#include <Client/source/editor/template/TileTemplate.h>
#include <Client/source/editor/tile/Wall.h>

namespace Editor
{
	class WallTemplate
		:
		public TileTemplate
	{
	public:
		WallTemplate()
			:
			TileTemplate()
		{
		}

		// onOpenContext ...

		// ex. can be saved and shon in context menu (portal binding)
		TileBase* create(
			sf::Vector2f position,
			sf::Vector2f size) override
		{
			TileSettings settings;

			settings.color = settings.color;
			settings.position = position;
			settings.size = size;

			return new Wall(settings);
		}
	};
}
