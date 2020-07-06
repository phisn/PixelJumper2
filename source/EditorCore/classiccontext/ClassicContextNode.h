#pragma once

#include "EditorDatabase.h"

#include "FrameworkCore/imgui/ImGuiModalWindow.h"
#include "ResourceCore/WorldResource.h"

#include <SFML/Graphics.hpp>

namespace Editor::ClassicContext
{
	class WorldNode;
	struct WindowAccess
	{
		virtual void beginLink(WorldNode* node) = 0;
		virtual void notifyWorldCreated(Resource::WorldID worldID) = 0;
	};

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

		virtual void draw(sf::RenderTarget* target) = 0;
		virtual void setStyle(NodeStyle style) = 0;

		virtual void setPosition(sf::Vector2f position) = 0;
		virtual sf::Vector2f getPosition() const = 0;

		// approx. max space this node can use to disable
		// drawing of unneeded nodes
		virtual sf::FloatRect getGlobalBounds() const = 0;
		virtual bool contains(sf::Vector2f point) const = 0;
	};
}