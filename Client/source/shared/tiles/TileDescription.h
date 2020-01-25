#pragma once

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
				[](const Resource::Tile* const tile,
				   const Resource::TileInstanceWrapper* const instance,
				   const Game::TileIdentity identity) -> Game::GameElementBase*
				{ 
					return GameTile::Create(tile, instance, identity); 
				},
				[]() -> Editor::TileTemplate* { return new EditorTemplate(); },
				[]() -> Resource::ResourceBase* { return new ResourceTile(); }
			};
		}

		Game::GameElementBase* (*createGameTile)(
			const Resource::Tile*, 
			const Resource::TileInstanceWrapper*, 
			const Game::TileIdentity);
		Editor::TileTemplate* (*createEditorTemplate)();
		Resource::ResourceBase* (*createResourceTile)();
	};

	struct TileDescription
	{
		static void Initialize();
		static void Uninitialize();

		static const TileDescription* Find(TileID tile);

		template <typename T = TileDescription>
		static const T * FindSpc(TileID tile)
		{
			return (T*)Find(tile);
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
