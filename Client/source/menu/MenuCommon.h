#pragma once

#include <SFML/Graphics/Color.hpp>

namespace Menu
{
	enum class CommonControlDirection
	{
		Horizontal,
		Vertical
	};

	enum class CommonControlEffect
	{
		Default,
		Hover,
		Pressed
	};

	struct CommonShapeStyle
	{
		sf::Color innerColor;
		sf::Color outerColor;

		float outlineThickness;
	};

	template <typename T = sf::Color>
	struct CommonControlStyle
	{
		T default, hover, pressed;
	};

	enum class CommonVerticalArea
	{
		Center,
		Top,
		Bottom
	};

	enum class CommonHorizontalArea
	{
		Center,
		Left,
		Right
	};

	enum class CommonContainerType
	{
		Raw,
		Compressed
	};
}