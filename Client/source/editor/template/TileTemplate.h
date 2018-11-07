#pragma once

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
			sf::Vector2f position) = 0;

		const sf::Color color;
		const std::wstring name;
	};
}