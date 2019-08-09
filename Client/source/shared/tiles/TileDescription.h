#pragma once

#include <Client/source/shared/tiles/TileId.h>

#include <Client/source/editor/tile/TileTemplate.h>
#include <Client/source/game/tiletrait/GameTileBase.h>
#include <Client/source/resource/TileResource.h>

#include <string>

#include <SFML/Graphics/Color.hpp>

namespace Shared
{
	struct TileCreation
	{
		template <
			typename GameTile,
			typename EditorTemplate,
			typename ResourceTile
		>
		static TileCreation Create()
		{
			return TileCreation{
				[](Resource::Tile * const tile) -> Game::GameTileBase* { return GameTile::Create(tile); }
				[]() -> Editor::TileTemplate* { return new EditorTemplate(); }
				[]() -> Editor::TileTemplate* { return new ResourceTile(); }
			};
		}

		Game::GameTileBase* (*createGameTile)(Resource::Tile*);
		Editor::TileTemplate* (*createEditorTemplate)();
		Resource::TileBase* (*createResourceTile)();
	};

	struct TileDescription
	{
		template <typename T = TileDescription>
		static const T* Find(TileId tile)
		{
			return (T*) Find<TileDescription>(tile);
		}

		TileDescription(
			const TileCreation tileCreation,

			const std::wstring name,
			const std::wstring info,
			const sf::Color gameColor,
			const sf::Color editorColor)
			:
			creation(tileCreation),
			name(name),
			info(info),
			gameColor(gameColor),
			editorColor(editorColor)
		{
		}

		std::wstring name, info;
		sf::Color gameColor, editorColor;
		TileCreation creation;
	};
}
