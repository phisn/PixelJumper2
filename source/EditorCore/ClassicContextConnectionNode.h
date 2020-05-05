#pragma once

#include "ClassicContextNode.h"
#include "ClassicContextConnection.h"

#include "FrameworkCore/BezierArrow.h"

namespace Editor
{
	class ClassicContextConnectionNode
		:
		public ClassicContextNode,
		public ClassicContextConnection
	{
	public:
		ClassicContextConnectionNode(
			ClassicContextWindowAccess* access,
			ClassicContextConnectionElement* source,
			ClassicContextConnectionElement* target)
			:
			ClassicContextConnection(source, target),
			access(access),
			sourceOut(true),
			targetOut(false)
		{
			arrow.setMode(Framework::BezierArrow::Mode::Default);
		}

		void draw(sf::RenderTarget* target)
		{
			target->draw(arrow);
		}

		Framework::IndependentPopupWindow* createPopupWindow() override
		{
			return NULL;
		}

		void setPosition(sf::Vector2f position) override
		{
		}

		void setStyle(ClassicContextNodeStyle style) override
		{
		}

		bool contains(sf::Vector2f point) const override
		{
			return false;
			// return shape.contains(point);
		}

		sf::FloatRect getGlobalBounds() const override
		{
			return { 0, 0, 0, 0 };
		}

		sf::Vector2f getPosition() const override
		{
			return { 0, 0 };
		}

		void setEndpointPosition(
			ClassicContextConnectionElement* world,
			ConnectionSide side,
			sf::Vector2f position)
		{
			if (world == sourceElement)
			{
				arrow.setSource(position);
				arrow.setSourceSide(side == ConnectionSide::Top || side == ConnectionSide::Bottom
					? Framework::BezierArrow::Side::Vertical
					: Framework::BezierArrow::Side::Horizontal);
			}
			else
			{
				assert(world == targetElement);

				arrow.setTarget(position);
				arrow.setTargetSide(side == ConnectionSide::Top || side == ConnectionSide::Bottom
					? Framework::BezierArrow::Side::Vertical
					: Framework::BezierArrow::Side::Horizontal);
			}
		}

		void setEndpointOut(
			ClassicContextConnectionElement* world,
			bool out)
		{
			assert(world == sourceElement || world == targetElement);

			bool& current = world == sourceElement
				? sourceOut
				: targetOut;

			if (current != out)
			{
				current = out;

				if (sourceOut && !targetOut)
				{
					arrow.setMode(Framework::BezierArrow::Mode::Default);
					return;
				}

				if (targetOut && !sourceOut)
				{
					arrow.setMode(Framework::BezierArrow::Mode::Reversed);
					return;
				}

				if (sourceOut && targetOut)
				{
					arrow.setMode(Framework::BezierArrow::Mode::Doubled);
					return;
				}

				access->removeLink(this);
			}
		}

		ClassicContextConnectionElement* getSourceElement()
		{
			return sourceElement;
		}

		ClassicContextConnectionElement* getTargetElement()
		{
			return targetElement;
		}

		Framework::BezierArrow::Mode getMode()
		{
			return arrow.getMode();
		}

	private:
		ClassicContextWindowAccess* access;

		bool sourceOut = false;
		bool targetOut = false;

		Framework::BezierArrow arrow;
	};
}
