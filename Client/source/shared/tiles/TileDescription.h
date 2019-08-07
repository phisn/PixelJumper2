#pragma once

#include <Client/source/shared/tiles/TileId.h>

#include <Client/source/editor/tile/TileTemplate.h>
#include <Client/source/game/tiletrait/GameTileBase.h>
#include <Client/source/resource/TileResource.h>

#include <string>

#include <SFML/Graphics/Color.hpp>

namespace Shared
{
	struct TileCollection
	{
		Game::GameTileBase* const gameTile;
		Editor::TileBase* const editorTile;
		Resource::TileBase* const resourceTile;
	};

	struct TileDescription
	{
		template <typename T = TileDescription>
		static const T* find(TileId tile)
		{
			return (T*) find<TileDescription>(tile);
		}

		template <>
		static const TileDescription* find<TileDescription>(TileId tile);

		TileDescription(
			const TileCollection tileCollection,

			const std::wstring name,
			const std::wstring info,
			const sf::Color gameColor,
			const sf::Color editorColor)
			:
			name(name),
			info(info),
			gameColor(gameColor),
			editorColor(editorColor)
		{
		}

		std::wstring name, info;
		sf::Color gameColor, editorColor;
	};
}
