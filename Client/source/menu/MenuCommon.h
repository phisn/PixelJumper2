#pragma once

#include <SFML/Graphics/Color.hpp>

namespace Menu
{
	struct CommonElementOffset
	{
		float left = 0.f, top = 0.f, right = 0.f, bottom = 0.f;
	};

	inline bool operator==(const CommonElementOffset& ceo1, const CommonElementOffset& ceo2)
	{
		return ceo1.left == ceo2.left &&
			ceo1.top == ceo2.top &&
			ceo1.right == ceo2.right &&
			ceo1.bottom == ceo2.bottom;
	}

	inline bool operator!=(const CommonElementOffset& ceo1, const CommonElementOffset& ceo2)
	{
		return !(ceo1 == ceo2);
	}

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