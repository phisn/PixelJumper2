#pragma once

#include <Client/source/editor/tile/TileBase.h>

#include <SFML/Graphics.hpp>

#include <string>

namespace Editor
{
	class TileTemplate
	{
	public:
		const struct Settings
		{
			std::wstring name;
			sf::Color color;

			int maxCount;

		};

		TileTemplate(
			const Settings settings)
			:
			// error implement posibility (settings constructor) to easyly pass settings
			settings(settings)
		{
		}

		const Settings* getSettings() const
		{
			return *settings;
		}

		virtual TileBase* create(
			sf::Vector2f position,
			sf::Vector2f size) = 0;
	protected:
		Settings settings;
	};
}