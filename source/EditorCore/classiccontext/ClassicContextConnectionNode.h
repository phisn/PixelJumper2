#pragma once

#include "ClassicContextNode.h"
#include "ClassicContextConnection.h"

#include "FrameworkCore/BezierArrow.h"

namespace Editor::ClassicContext
{
	class ConnectionNode
		:
		public Node,
		public Connection
	{
	public:
		ConnectionNode(
			WindowAccess* access,
			ConnectionElement* source,
			ConnectionElement* target)
			:
			Connection(source, target),
			access(access),
			sourceOut(true),
			targetOut(false)
		{
			arrow.setMode(Framework::StrictBezierArrow::Mode::Default);
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

		void setStyle(NodeStyle style) override
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
			ConnectionElement* world,
			ConnectionSide side,
			sf::Vector2f position)
		{
			if (world == sourceElement)
			{
				arrow.setSource(position);
				arrow.setSourceSide(side == ConnectionSide::Top || side == ConnectionSide::Bottom
					? Framework::StrictBezierArrow::Side::Vertical
					: Framework::StrictBezierArrow::Side::Horizontal);
			}
			else
			{
				assert(world == targetElement);

				arrow.setTarget(position);
				arrow.setTargetSide(side == ConnectionSide::Top || side == ConnectionSide::Bottom
					? Framework::StrictBezierArrow::Side::Vertical
					: Framework::StrictBezierArrow::Side::Horizontal);
			}
		}

		void setEndpointOut(
			ConnectionElement* world,
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
					arrow.setMode(Framework::StrictBezierArrow::Mode::Default);
					return;
				}

				if (targetOut && !sourceOut)
				{
					arrow.setMode(Framework::StrictBezierArrow::Mode::Reversed);
					return;
				}

				if (sourceOut && targetOut)
				{
					arrow.setMode(Framework::StrictBezierArrow::Mode::Doubled);
					return;
				}

				access->removeLink(this);
			}
		}

		ConnectionElement* getSourceElement()
		{
			return sourceElement;
		}

		ConnectionElement* getTargetElement()
		{
			return targetElement;
		}

		Framework::StrictBezierArrow::Mode getMode()
		{
			return arrow.getMode();
		}

		bool getElementOutput(ConnectionElement* element) const
		{
			if (element == sourceElement)
			{
				return sourceOut;
			}

			if (element == targetElement)
			{
				return targetOut;
			}

			return false;
		}

	private:
		WindowAccess* access;

		bool sourceOut = false;
		bool targetOut = false;

		Framework::StrictBezierArrow arrow;
	};
}