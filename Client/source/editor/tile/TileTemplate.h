#pragma once

#include <Client/source/Common.h>
#include <Client/source/editor/tile/EditorTileBase.h>
#include <Client/source/menu/MenuElementBase.h>

#include <SFML/Graphics/Color.hpp>

#include <string>

namespace Editor
{
	class TileTemplate
	{
	public:
		virtual TileBase* create(const sf::Vector2f position) = 0;

		virtual const std::wstring& getName() const = 0;
		virtual const std::wstring& getDescription() const = 0;
		virtual Shared::TileId getId() const = 0;

		virtual bool isSelected() const = 0;

		virtual void select() = 0;
		virtual void unselect() = 0;

		// usally a button
		virtual Menu::ElementBase* createRepresentation() = 0;
	};
}