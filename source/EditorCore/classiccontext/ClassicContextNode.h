#pragma once

#include "ClassicContextPopup.h"
#include "EditorDatabase.h"

#include "FrameworkCore/imgui/ImGuiModalWindow.h"
#include "ResourceCore/ResourceBase.h"

#include <SFML/Graphics.hpp>

namespace Editor::ClassicContext
{
	enum class NodeStyle
	{
		Classic,
		Hover,
		Selected,

		_Length
	};

	// nodes do not have to have a rect form
	// nodes are worlds, arrows, groups, ...
	struct Node
		:
		public Resource::ResourceBase
	{
		virtual ~Node()
		{
		}

		virtual Framework::IndependentPopupWindow* createPopupWindow(
			WindowAccess* access) = 0;

		virtual void setStyle(NodeStyle style) = 0;

		virtual void setPosition(sf::Vector2f position) = 0;
		virtual sf::Vector2f getPosition() const = 0;

		// approx. max space this node can use to disable
		// drawing of unneeded nodes
		virtual sf::FloatRect getGlobalBounds() const = 0;
		virtual bool contains(sf::Vector2f point) const = 0;
	};
}