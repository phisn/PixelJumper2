#pragma once

#include <string>

#include <SFML/Graphics/Color.hpp>

namespace Shared
{
	struct TileDescription
	{
		std::wstring name, info;

		sf::Color gameColor, editorColor;
	};
}
