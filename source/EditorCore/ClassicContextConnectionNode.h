#pragma once

#include "FrameworkCore/ArrowShape.h"
#include "ClassicContextNode.h"

namespace Editor
{
	class ClassicContextConnectionNode
		:
		public ClassicContextNode
	{
	public:
		Framework::IndependentPopupWindow* createPopupWindow(
			ClassicContextWindowAccess* access) override
		{
		}

		void setPosition(sf::Vector2f position) override
		{
		}

		void setStyle(ClassicContextNodeStyle style) override
		{
		}

		bool contains(sf::Vector2f point) const override
		{
			return shape.contains(point);
		}

		sf::FloatRect getGlobalBounds() override
		{
			return shape.getGlobalBounds();
		}

		sf::Vector2f getPosition() const override
		{
			return shape.getPosition();
		}

		void setEndpointPosition(sf::Vector2f position)
		{
		}

	private:
		Framework::ArrowShape shape;
	};
}
