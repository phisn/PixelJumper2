#pragma once

#include <SFML/Graphics/Color.hpp>

namespace Menu
{
	struct CommonElementOffset
	{
		float left = 0.f, top = 0.f, right = 0.f, bottom = 0.f;
	};

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

	enum class CommonSelectedEffect
	{
		Selected,
		SelectedHover,
		SelectedPressed
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

	/* Deptrecated

	enum class CommonContainerType
	{
		Raw,
		Compressed
	};*/
}