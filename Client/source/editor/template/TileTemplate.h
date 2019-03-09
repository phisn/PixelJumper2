#pragma once

#include <Client/source/Common.h>
#include <Client/source/editor/tile/EditorTileBase.h>

#include <SFML/Graphics.hpp>

#include <string>

namespace Editor
{
	class TileTemplate
	{
	public:
		TileTemplate(
			const sf::Color color,
			const std::wstring name,
			const Game::TileId id)
			:
			color(color),
			name(name),
			id(id)
		{
		}

		virtual TileBase* create(
			const VectorTilePosition position) = 0;

		const sf::Color color;
		const std::wstring name;
		const Game::TileId id;
	};
}