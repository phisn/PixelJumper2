#pragma once

#include "FrameworkCore/BezierArrow.h"
#include "ClassicContextNode.h"

namespace Editor
{
	enum class ConnectionSide
	{
		Top,
		Left,
		Bottom,
		Right
	};

	class ClassicContextConnectionNode
		:
		public ClassicContextNode
	{
	public:
		ClassicContextConnectionNode(
			ClassicContextWindowAccess* access,
			ClassicWorldDataset* source,
			ClassicWorldDataset* target)
			:
			access(access),
			sourceWorld(source),
			targetWorld(target),
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

		sf::FloatRect getGlobalBounds() override
		{
			return { 0, 0, 0, 0 };
		}

		sf::Vector2f getPosition() const override
		{
			return { 0, 0 };
		}

		void setEndpointPosition(
			ClassicWorldDataset* world,
			ConnectionSide side,
			sf::Vector2f position)
		{
			if (world == sourceWorld)
			{
				arrow.setSource(position);
				arrow.setSourceSide(side == ConnectionSide::Top || side == ConnectionSide::Bottom
					? Framework::BezierArrow::Side::Vertical
					: Framework::BezierArrow::Side::Horizontal);
			}
			else
			{
				assert(world == targetWorld);

				arrow.setTarget(position);
				arrow.setTargetSide(side == ConnectionSide::Top || side == ConnectionSide::Bottom
					? Framework::BezierArrow::Side::Vertical
					: Framework::BezierArrow::Side::Horizontal);
			}
		}

		void setEndpointOut(
			ClassicWorldDataset* world,
			bool out)
		{
			assert(world == sourceWorld || world == targetWorld);

			bool& current = world == sourceWorld
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

		ClassicWorldDataset* getSourceWorld()
		{
			return sourceWorld;
		}

		ClassicWorldDataset* getTargetWorld()
		{
			return targetWorld;
		}

		Framework::BezierArrow::Mode getMode()
		{
			return arrow.getMode();
		}

	private:
		ClassicContextWindowAccess* access;

		ClassicWorldDataset* sourceWorld;
		bool sourceOut;

		ClassicWorldDataset* targetWorld;
		bool targetOut;

		Framework::BezierArrow arrow;
	};
}
