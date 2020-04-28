#pragma once

#include "ClassicContextWorldNode.h"

#include "FrameworkCore/ArrowShape.h"

namespace Editor
{
	class ClassicContextArrowNode
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

	private:
		Framework::ArrowShape shape;
	};
}
