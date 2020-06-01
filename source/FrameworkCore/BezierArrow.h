#pragma once

#include "SFML/Graphics.hpp"

#include <cassert>

namespace Framework
{
	class BezierCurve
		:
		public sf::Drawable
	{
	public:
		BezierCurve()
			:
			BezierCurve(5.f, 200)
		{
		}

		BezierCurve(float width, int pointCount)
			:
			vertex(sf::PrimitiveType::TriangleStrip),
			width(width),
			pointCount(pointCount)
		{
		}

		void setSource(sf::Vector2f point)
		{
			source = point;
			needsUpdate = true;
		}

		void setTarget(sf::Vector2f point)
		{
			target = point;
			needsUpdate = true;
		}

		void setP0(sf::Vector2f point)
		{
			p0 = point;
			needsUpdate = true;
		}

		void setP1(sf::Vector2f point)
		{
			p1 = point;
			needsUpdate = true;
		}

		void setColor(sf::Color color)
		{
			this->color = color;
			needsUpdate = true;
		}

		void setPointCount(int pointCount)
		{
			this->pointCount = pointCount;
			needsUpdate = true;
		}

		void setWidth(float width)
		{
			this->width = width;
			needsUpdate = true;
		}

		sf::Vector2f getSource() const
		{
			return source;
		}

		sf::Vector2f getTarget() const
		{
			return target;
		}

		sf::Vector2f getP0() const
		{
			return p0;
		}

		sf::Vector2f getP1() const
		{
			return p1;
		}

		float getWidth() const
		{
			return width;
		}

	protected:
		mutable bool needsUpdate = true;
		mutable sf::VertexArray vertex;

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override
		{
			if (needsUpdate)
			{
				needsUpdate = false;
				construct();
			}

			target.draw(vertex, states);
		}

	private:
		sf::Color color = sf::Color::White;
		
		int pointCount;
		float width;

		sf::Vector2f source;
		sf::Vector2f target;

		sf::Vector2f p0, p1;

		void construct() const
		{
			assert(pointCount > 0);
			assert(width > 0);

			vertex.clear();

			sf::Vector2f direction = target - source;
			float directionLength = sqrtf(direction.x * direction.x + direction.y * direction.y);

			std::vector<sf::Vector2f> bezier;
			bezier.reserve(pointCount);

			float minDistance = directionLength / (pointCount * 2);
			float step = 1.f / (pointCount - 1);

			for (int i = 0; i < pointCount; ++i)
			{
				float t = i * step;
				float inv_t = 1 - t;

				sf::Vector2f bezierPoint =
					source * (inv_t * inv_t * inv_t) +
					p0 * (3 * inv_t * inv_t * t) +
					p1 * (3 * inv_t * t * t) +
					target * (t * t * t);

				if (bezier.size() > 0)
				{
					sf::Vector2f bezierPointDifference = bezierPoint - bezier.back();

					if (sqrtf(bezierPointDifference.x * bezierPointDifference.x +
						bezierPointDifference.y * bezierPointDifference.y) < minDistance)
						continue;
				}

				bezier.push_back(bezierPoint);
			}

			sf::Vector2f sourceDirection = p0 - source;
			sf::Vector2f targetDirection = p1 - target;

			float sourceAngle = atan2(sourceDirection.y, sourceDirection.x);
			float targetAngle = atan2(targetDirection.y, targetDirection.x);

			sf::Vector2f sourceP0 = sf::Vector2f{ -sin(sourceAngle), cos(sourceAngle) } * width / 2.f;
			sf::Vector2f sourceP1 = -sourceP0;

			sf::Vector2f targetP0 = sf::Vector2f{ -sin(targetAngle), cos(targetAngle) } * width / 2.f;
			sf::Vector2f targetP1 = -targetP0;

			pushVertex(source + sourceP0);
			pushVertex(source + sourceP1);
			vertex.append(vertex[vertex.getVertexCount() - 1]);

			for (int bezierIndex = 1; bezierIndex < bezier.size() - 1; ++bezierIndex)
			{
				sf::Vector2f p0_to_pn = bezier[bezierIndex] - bezier[bezierIndex - 1];
				sf::Vector2f p0_to_p1 = bezier[bezierIndex + 1] - bezier[bezierIndex - 1];

				float squared_p0_to_p1 = p0_to_p1.x * p0_to_p1.x + p0_to_p1.y * p0_to_p1.y;
				float p0pn_dot_p0p1 = p0_to_pn.x * p0_to_p1.x + p0_to_pn.y * p0_to_p1.y;

				float distance = p0pn_dot_p0p1 / squared_p0_to_p1;

				sf::Vector2f outer_direction_position =
				{
					bezier[bezierIndex - 1].x + p0_to_p1.x * distance,
					bezier[bezierIndex - 1].y + p0_to_p1.y * distance
				};

				sf::Vector2f outer_direction = bezier[bezierIndex] - outer_direction_position;
				float outer_length = sqrtf(outer_direction.x * outer_direction.x + outer_direction.y * outer_direction.y);

				float p0_scale = (outer_length + width / 2.f) / outer_length;
				float p1_scale = (outer_length - width / 2.f) / outer_length;

				sf::Vector2f vertex0 = outer_direction_position + outer_direction * p0_scale;
				sf::Vector2f vertex1 = outer_direction_position + outer_direction * p1_scale;

				if (!std::isnan(vertex0.x + vertex0.y + vertex1.x + vertex1.y))
					pushVertexConstruct(vertex0, vertex1);
			}

			pushVertexConstruct(target + targetP0, target + targetP1);
		}

		void pushVertex(sf::Vector2f point) const
		{
			vertex.append(sf::Vertex{ point, color });
		}

		void pushVertexConstruct(sf::Vector2f p0, sf::Vector2f p1) const
		{
			vertex.append(sf::Vertex(p0, color));
			vertex.append(sf::Vertex(vertex[vertex.getVertexCount() - 4].position, color));
			vertex.append(sf::Vertex(p1, color));
			vertex.append(sf::Vertex(vertex[vertex.getVertexCount() - 4].position, color));
			vertex.append(sf::Vertex(p0, color));
		}
	};

	class BezierArrow
		:
		public sf::Drawable
	{
	public:
		enum Mode
		{
			Disabled,
			Default,
			Reversed,
			Doubled
		};

		BezierArrow()
			:
			BezierArrow(Doubled, 5.f, 200)
		{
		}

		BezierArrow(Mode mode, float width, int pointCount)
			:
			curve(width, pointCount),
			vertex(sf::PrimitiveType::Triangles),
			mode(mode)
		{
			sf::Vector2f source = { 0, 0 };
			sf::Vector2f target = { 240, -260 };

			sf::Vector2f p0 = { 0, -200 }, p1 = { 140, -200 };

			setSource(source);
			setTarget(target);
			setP0(p0);
			setP1(p1);
		}

		void setMode(Mode mode)
		{
			this->mode = mode;

			setSource(source);
			setTarget(target);
		}

		void setSource(sf::Vector2f point)
		{
			source = point;
			curve.setSource(point + (mode == Reversed || mode == Doubled
				? makeHeadOffset(point, getP0())
				: sf::Vector2f{ }));
			needsUpdate = true;
		}

		void setTarget(sf::Vector2f point)
		{
			target = point;
			curve.setTarget(point + (mode == Default || mode == Doubled
				? makeHeadOffset(point, getP1())
				: sf::Vector2f{ }));
			needsUpdate = true;
		}

		void setP0(sf::Vector2f point)
		{
			curve.setP0(point);
			setSource(source);
		}

		void setP1(sf::Vector2f point)
		{
			curve.setP1(point);
			setTarget(target);
		}

		void setBodyColor(sf::Color color)
		{
			curve.setColor(color);
		}

		void setPointCount(int pointCount)
		{
			curve.setPointCount(pointCount);
		}

		void setWidth(float width)
		{
			curve.setWidth(width);
			needsUpdate = true;
		}

		void setSourceHeadColor(sf::Color color)
		{
			sourceColor = color;
			needsUpdate = true;
		}

		void setTargetHeadColor(sf::Color color)
		{
			targetColor = color;
			needsUpdate = true;
		}

		Mode getMode() const
		{
			return mode;
		}

		sf::Vector2f getSource() const
		{
			return source;
		}

		sf::Vector2f getTarget() const
		{
			return target;
		}

		sf::Vector2f getP0() const
		{
			return curve.getP0();
		}

		sf::Vector2f getP1() const
		{
			return curve.getP1();
		}

		float getWidth() const
		{
			return curve.getWidth();
		}

	protected:
		mutable sf::VertexArray vertex;
		mutable bool needsUpdate = true;

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override
		{
			if (needsUpdate)
			{
				construct();
				needsUpdate = false;
			}

			target.draw(curve, states);
			target.draw(vertex, states);
		}

	private:
		sf::Color sourceColor = sf::Color::White;
		sf::Color targetColor = sf::Color::White;

		sf::Vector2f source;
		sf::Vector2f target;

		Mode mode;

		float headWidthRatio = 0.7f;
		float headLengthRatio = 1.0f;

		BezierCurve curve;

		void construct() const
		{
			vertex.clear();

			if (mode == Disabled)
				return;

			float headWidth = 2 * headWidthRatio * getWidth() / (1 - headWidthRatio);

			if (mode == Reversed || mode == Doubled)
			{
				sf::Vector2f sourceDirection = curve.getSource() - source;
				float sourceAngle = atan2(sourceDirection.y, sourceDirection.x);

				sf::Vector2f sourceP0 = sf::Vector2f{ -sin(sourceAngle), cos(sourceAngle) } * headWidth / 2.f;
				sf::Vector2f sourceP1 = -sourceP0;

				appendTriangle(
					source,
					curve.getSource() + sourceP0,
					curve.getSource() + sourceP1,
					sourceColor);
			}

			if (mode == Default || mode == Doubled)
			{
				sf::Vector2f targetDirection = curve.getTarget() - target;
				float targetAngle = atan2(targetDirection.y, targetDirection.x);

				sf::Vector2f targetP0 = sf::Vector2f{ -sin(targetAngle), cos(targetAngle) } * headWidth / 2.f;
				sf::Vector2f targetP1 = -targetP0;

				appendTriangle(
					target,
					curve.getTarget() + targetP0,
					curve.getTarget() + targetP1,
					targetColor);
			}
		}

		void appendTriangle(
			sf::Vector2f p0, 
			sf::Vector2f p1, 
			sf::Vector2f p2,
			sf::Color color) const
		{
			vertex.resize(vertex.getVertexCount() + 3);

			vertex[vertex.getVertexCount() - 3].position = p0;
			vertex[vertex.getVertexCount() - 3].color = color;
			vertex[vertex.getVertexCount() - 2].position = p1;
			vertex[vertex.getVertexCount() - 2].color = color;
			vertex[vertex.getVertexCount() - 1].position = p2;
			vertex[vertex.getVertexCount() - 1].color = color;
		}

		sf::Vector2f makeHeadOffset(sf::Vector2f p0, sf::Vector2f p1) const
		{
			sf::Vector2f direction = p1 - p0;
			float headLength = headLengthRatio * getWidth() / (1 - headWidthRatio);
			float directionToOffsetScale = headLength / sqrt(direction.x * direction.x + direction.y * direction.y);
			return direction * directionToOffsetScale;
		}
	};

	class StrictBezierArrow
		:
		public sf::Drawable
	{
	public:
		typedef BezierArrow::Mode Mode;

		enum class Side
		{
			Vertical, Horizontal
		};

		StrictBezierArrow()
		{
		}

		void setWidth(float width)
		{
			arrow.setWidth(width);
		}

		void setSource(sf::Vector2f source)
		{
			arrow.setSource(source);
			updateCurvePositions();
		}

		void setTarget(sf::Vector2f target)
		{
			arrow.setTarget(target);
			updateCurvePositions();
		}

		void setMode(Mode mode)
		{
			arrow.setMode(mode);
		}

		void setSourceSide(Side side)
		{
			sourceSide = side;
			updateCurvePositions();
		}

		void setTargetSide(Side side)
		{
			targetSide = side;
			updateCurvePositions();
		}

		void setBodyColor(sf::Color color)
		{
			arrow.setBodyColor(color);
		}

		Mode getMode() const
		{
			return arrow.getMode();
		}

		sf::Vector2f getSource() const
		{
			return arrow.getSource();
		}

		sf::Vector2f getTarget() const
		{
			return arrow.getTarget();
		}

	protected:
		mutable bool needsUpdate = true;

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override
		{
			target.draw(arrow, states);
		}

	private:
		BezierArrow arrow;

		Side sourceSide;
		Side targetSide;

		void updateCurvePositions()
		{
			sf::Vector2f source = arrow.getSource();
			sf::Vector2f target = arrow.getTarget();

			if (sourceSide == targetSide)
			{
				float strength = 0.7;
				sf::Vector2f p0, p1;

				if (sourceSide == Side::Vertical)
				{
					p0 = { source.x, target.y * strength + source.y * (1 - strength) };
					p1 = { target.x, source.y * strength + target.y * (1 - strength) };
				}
				else
				{
					p0 = { target.x * strength + source.x * (1 - strength), source.y };
					p1 = { source.x * strength + target.x * (1 - strength), target.y };
				}

				arrow.setP0(p0);
				arrow.setP1(p1);
			}
			else
			{
				float strength = 0.7;
				sf::Vector2f p0, p1;

				if (sourceSide == Side::Vertical)
				{
					p0 = { source.x, target.y * strength + source.y * (1 - strength) };
					p1 = { source.x * strength + target.x * (1 - strength), target.y };
				}
				else
				{
					p0 = { target.x * strength + source.x * (1 - strength), source.y };
					p1 = { target.x, source.y * strength + target.y * (1 - strength) };
				}

				arrow.setP0(p0);
				arrow.setP1(p1);
			}
		}
	};
}
