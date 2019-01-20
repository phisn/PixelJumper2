#pragma once

#include <Client/source/Common.h>
#include <Client/source/editor/tile/TileBase.h>

#include <SFML/Graphics.hpp>

#include <string>

namespace Editor
{
	class TileTemplate
	{
	public:
		TileTemplate(
			const sf::Color color,
			const std::wstring name)
			:
			color(color),
			name(name)
		{
		}

		virtual TileBase* create(
			const VectorTilePosition position) = 0;

		const sf::Color color;
		const std::wstring name;
	};
}