#pragma once

#include <Client/source/editor/tile/EditorTileBase.h>

#include <SFML/Graphics/Rect.hpp>

#include <vector>

namespace Editor
{
	enum { DefaultConverterPriority = 0 };

	enum class SelectorType
	{
		None,
		Area,
		Tiles
	};

	class SelectorInput
	{
	public:
		SelectorType type;

		sf::Vector2f beginCorner, nextCorner;
	};

	class SelectorOutput
	{
	public:
		SelectorType type;

		std::vector<TileBase*> tiles;
		sf::FloatRect area;
	};
}
