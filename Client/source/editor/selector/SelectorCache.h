#pragma once

#include <SFML/Graphics/Rect.hpp>

#include <vector>

namespace Editor
{
	enum { DefaultConverterPriority = 0 };

	enum class SelectorType
	{
		Area,
		Tiles
	};

	class SelectorInput
	{
	public:
		SelectorType type;
		sf::FloatRect selectRect;
	};

	class SelectorOutput
	{
	public:
		SelectorType type;

		std::vector<void> tiles;
		sf::FloatRect area;
	};
}
