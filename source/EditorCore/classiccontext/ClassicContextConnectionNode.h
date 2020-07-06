#pragma once

#include "ClassicContextNode.h"
#include "ClassicContextConnection.h"
#include "ClassicContextWorldNode.h"

#include "FrameworkCore/BezierArrow.h"

namespace Editor::ClassicContext
{
	class ConnectionNode
		:
		public Node,
		public Connection
	{
		struct Style
		{
			sf::Color targetHeadColor;
		};

		const Style styles[(int)NodeStyle::_Length] =
		{
			Style { sf::Color::Color(255, 255, 255) },
			Style { sf::Color::Color(195, 195, 195) },
			Style { sf::Color::Color(135, 135, 135) }
		};

	public:
		ConnectionNode(
			WorldNode* source,
			WorldNode* target)
			:
			Connection(source, target),
			source(source),
			target(target),
			sourceOut(true),
			targetOut(false)
		{
			arrow.setMode(Framework::StrictBezierArrow::Mode::Default);
		}
		
		virtual bool make(Resource::ReadPipe* const pipe) override
		{
			return true;
		}

		virtual bool save(Resource::WritePipe* const pipe) override
		{
			return true;
		}

		void draw(sf::RenderTarget* target) override
		{
			target->draw(arrow);
		}

		void setPosition(sf::Vector2f position) override
		{
		}

		void setStyle(NodeStyle style) override
		{
			arrow.setTargetHeadColor(styles[(int) style].targetHeadColor);
		}

		bool contains(sf::Vector2f point) const override
		{
			return arrow.contains(point) == Framework::ArrowContain::Target;
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

				arrow.setMode(Framework::StrictBezierArrow::Mode::Disabled);
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

		WorldNode* getSource() const
		{
			return source;
		}

		WorldNode* getTarget() const
		{
			return target;
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
		WorldNode* source;
		WorldNode* target;

		bool sourceOut = false;
		bool targetOut = false;

		Framework::StrictBezierArrow arrow;
	};
}
