#pragma once

#include "SFML/Graphics.hpp"

#include "Logger/Logger.h"

#include <cassert>

namespace Framework
{
	class BezierArrow
		:
		public sf::Drawable
	{
	public:
		enum class Side
		{
			Vertical, Horizontal
		};

		enum class Mode
		{
			Default, Reversed, Doubled
		};

		BezierArrow()
			:
			vertex{ sf::PrimitiveType::TriangleStrip }
		{
		}

		void setSource(sf::Vector2f source)
		{
			this->source = source;
			needsUpdate = true;
		}

		void setTarget(sf::Vector2f target)
		{
			this->target = target;
			needsUpdate = true;
		}

	private:
		mutable sf::VertexArray vertex;

		float headWidthRatio = 0.7f;
		float headLengthRatio = 1.0f;

		float width = 8.f;
		float density = 1.f;

		sf::Vector2f source = { 0, 0 };
		sf::Vector2f target = { 200, 200 };

		Side sourceSide = Side::Horizontal;
		Side targetSide = Side::Horizontal;

		Mode mode = Mode::Default;
		
		sf::Color bodyColor = sf::Color::White;

		mutable bool needsUpdate = true;

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override
		{
			if (needsUpdate)
			{
				needsUpdate = false;
				construct();
			}

			target.draw(vertex, states);
		}

		void construct() const
		{
			vertex.clear();

			float headWidth = width;
			float headLength = width * headLengthRatio;

			if (mode != Mode::Default)
			{
				vertex.append(sf::Vertex{ source, bodyColor });

				if (sourceSide == Side::Vertical)
				{
					if (source.y < target.y)
					{
						vertex.append(sf::Vertex{ source + sf::Vector2f{  headWidth / 2.f, headLength }, bodyColor });
						vertex.append(sf::Vertex{ source + sf::Vector2f{ -headWidth / 2.f, headLength }, bodyColor });
					}
					else
					{
						vertex.append(sf::Vertex{ source + sf::Vector2f{  headWidth / 2.f, -headLength }, bodyColor });
						vertex.append(sf::Vertex{ source + sf::Vector2f{ -headWidth / 2.f, -headLength }, bodyColor });
					}
				}
				else
				{
					if (source.x < target.x)
					{
						vertex.append(sf::Vertex{ source + sf::Vector2f{  headLength,  headWidth / 2.f }, bodyColor });
						vertex.append(sf::Vertex{ source + sf::Vector2f{  headLength, -headWidth / 2.f }, bodyColor });
					}
					else
					{
						vertex.append(sf::Vertex{ source + sf::Vector2f{  -headLength,  headWidth / 2.f }, bodyColor });
						vertex.append(sf::Vertex{ source + sf::Vector2f{  -headLength, -headWidth / 2.f }, bodyColor });
					}
				}
			}

			sf::Vector2f bezierSource;
			if (mode != Mode::Default)
			{
				bezierSource = source + (sourceSide == Side::Vertical
					? sf::Vector2f{ 0, headLength }
					: sf::Vector2f{ headLength, 0 }) *
					(source.y > target.y ? -1.f : 1.f);
			}
			else
			{
				bezierSource = source;
			}

			sf::Vector2f bezierTarget;
			if (mode != Mode::Reversed)
			{
				bezierTarget = target + (targetSide == Side::Vertical
					? sf::Vector2f{ 0, headLength }
					: sf::Vector2f{ headLength, 0 }) *
					(target.y > source.y ? -1.f : 1.f);
			}
			else
			{
				bezierTarget = target;
			}

			float bezierWidth = (1 - headWidthRatio) * width;

			sf::Vector2f bezierDirection = bezierTarget - bezierSource;
			float bezierDirectionLength = sqrtf(bezierDirection.x * bezierDirection.x + bezierDirection.y * bezierDirection.y);

			float bezierPointsCount = std::ceil(bezierDirectionLength * density);

			std::vector<sf::Vector2f> bezierPoints;
			bezierPoints.reserve(bezierPointsCount);

			if (sourceSide == targetSide)
			{
				sf::Vector2f p0, p1;

				if (sourceSide == Side::Vertical)
				{
					p0 = { bezierSource.x, bezierTarget.y };
					p1 = { bezierTarget.x, bezierSource.y };
				}
				else
				{
					p0 = { bezierTarget.x, bezierSource.y };
					p1 = { bezierSource.x, bezierTarget.y };
				}

				float step = 1.f / (bezierPointsCount - 1);
				for (int i = 0; i < bezierPointsCount; ++i)
				{
					float t = i * step;
					float inv_t = 1 - t;

					sf::Vector2f bezierPoint =
						bezierSource * (inv_t * inv_t * inv_t) +
						p0 * (3 * inv_t * inv_t * t) +
						p1 * (3 * inv_t * t * t) +
						bezierTarget * (t * t * t);

					bezierPoints.push_back(bezierPoint);
				}
			}
			else
			{
				sf::Vector2f p0 = sourceSide == Side::Vertical
					? sf::Vector2f{ bezierTarget.y, bezierSource.x }
					: sf::Vector2f{ bezierSource.y, bezierTarget.x };

				float step = 1.f / (bezierPointsCount - 1);
				for (int i = 0; i < bezierPointsCount; ++i)
				{
					float t = i * step;
					float inv_t = 1 - t;

					sf::Vector2f bezierPoint =
						bezierSource * (inv_t * inv_t) +
						p0 * (2 * inv_t * t) + 
						bezierTarget * (t * t);

					bezierPoints.push_back(bezierPoint);
				}
			}

			if (sourceSide == Side::Vertical)
			{
				if (bezierSource.y > bezierTarget.y)
				{
					vertex.append(sf::Vertex(bezierSource + sf::Vector2f{ bezierWidth / 2.f, 0 }, bodyColor));
					vertex.append(sf::Vertex(bezierSource + sf::Vector2f{ -bezierWidth / 2.f, 0 }, bodyColor));
				}
				else
				{
					vertex.append(sf::Vertex(bezierSource + sf::Vector2f{ -bezierWidth / 2.f, 0 }, bodyColor));
					vertex.append(sf::Vertex(bezierSource + sf::Vector2f{ bezierWidth / 2.f, 0 }, bodyColor));
				}
			}
			else
			{
				if (bezierSource.x > bezierTarget.x)
				{
					vertex.append(sf::Vertex(bezierSource + sf::Vector2f{ 0,  bezierWidth / 2.f }, bodyColor));
					vertex.append(sf::Vertex(bezierSource + sf::Vector2f{ 0, -bezierWidth / 2.f }, bodyColor));
				}
				else
				{
					vertex.append(sf::Vertex(bezierSource + sf::Vector2f{ 0, -bezierWidth / 2.f }, bodyColor));
					vertex.append(sf::Vertex(bezierSource + sf::Vector2f{ 0,  bezierWidth / 2.f }, bodyColor));
				}
			}

			vertex.append(vertex[vertex.getVertexCount() - 1]);
			
			// skip first and last bezier point
			int bezier_index = 1;

			while (bezier_index < bezierPointsCount - 1)
			{
				// bezierPoints[bezier_index + 1] + bezierPoints[bezier_index - 1]
				
				sf::Vector2f p0_to_pn = bezierPoints[bezier_index] - bezierPoints[bezier_index - 1];
				sf::Vector2f p0_to_p1 = bezierPoints[bezier_index + 1] - bezierPoints[bezier_index - 1];

				float squared_p0_to_p1 = p0_to_p1.x * p0_to_p1.x + p0_to_p1.y * p0_to_p1.y;
				float p0pn_dot_p0p1 = p0_to_pn.x * p0_to_p1.x + p0_to_pn.y * p0_to_p1.y;

				float distance = p0pn_dot_p0p1 / squared_p0_to_p1;

				sf::Vector2f outer_direction_position = 
				{ 
					bezierPoints[bezier_index - 1].x + p0_to_p1.x * distance,
					bezierPoints[bezier_index - 1].y + p0_to_p1.y * distance
				};

				sf::Vector2f outer_direction = bezierPoints[bezier_index] - outer_direction_position;
				float outer_length = sqrtf(outer_direction.x * outer_direction.x + outer_direction.y * outer_direction.y);

				float p0_scale = (outer_length + bezierWidth / 2.f) / outer_length;
				float p1_scale = (outer_length - bezierWidth / 2.f) / outer_length;

				sf::Vector2f vertex0, vertex1;

				if ((int) (outer_direction.y * 10000) <= 0)
				{
					vertex0 = outer_direction_position + outer_direction * p1_scale;
					vertex1 = outer_direction_position + outer_direction * p0_scale;
				}
				else
				{
					vertex0 = outer_direction_position + outer_direction * p0_scale;
					vertex1 = outer_direction_position + outer_direction * p1_scale;
				}

				if (!std::isnan(vertex0.x + vertex0.y + vertex1.x + vertex1.y))
				{
					vertex.append(sf::Vertex(vertex0, bodyColor));
					vertex.append(sf::Vertex(vertex[vertex.getVertexCount() - 4].position, bodyColor));
					vertex.append(sf::Vertex(vertex1, bodyColor));
					vertex.append(sf::Vertex(vertex[vertex.getVertexCount() - 4].position, bodyColor));
					vertex.append(sf::Vertex(vertex0, bodyColor));
				}

				++bezier_index;
			}

			sf::Vector2f vertex_end0, vertex_end1;

			if (targetSide == Side::Vertical)
			{
				vertex_end0 = bezierTarget + sf::Vector2f{ bezierWidth / 2.f, 0 };
				vertex_end1 = bezierTarget + sf::Vector2f{ -bezierWidth / 2.f, 0 };
			}
			else
			{
				vertex_end0 = bezierTarget + sf::Vector2f{ 0,  bezierWidth / 2.f };
				vertex_end1 = bezierTarget + sf::Vector2f{ 0, -bezierWidth / 2.f };
			}

			vertex.append(sf::Vertex(vertex_end0, bodyColor));
			vertex.append(sf::Vertex(vertex[vertex.getVertexCount() - 4].position, bodyColor));
			vertex.append(sf::Vertex(vertex_end1, bodyColor));
			vertex.append(sf::Vertex(vertex[vertex.getVertexCount() - 4].position, bodyColor));
			vertex.append(sf::Vertex(vertex_end0, bodyColor));

			if (mode != Mode::Reversed)
			{
				vertex.append(sf::Vertex{ target, bodyColor });

				if (sourceSide == Side::Vertical)
				{
					if (source.y > target.y)
					{
						vertex.append(sf::Vertex{ target + sf::Vector2f{  headWidth / 2.f, headLength }, bodyColor });
						vertex.append(sf::Vertex{ target + sf::Vector2f{ -headWidth / 2.f, headLength }, bodyColor });
					}
					else
					{
						vertex.append(sf::Vertex{ target + sf::Vector2f{  headWidth / 2.f, -headLength }, bodyColor });
						vertex.append(sf::Vertex{ target + sf::Vector2f{ -headWidth / 2.f, -headLength }, bodyColor });
					}
				}
				else
				{
					if (source.x > target.x)
					{
						vertex.append(sf::Vertex{ target + sf::Vector2f{  headLength,  headWidth / 2.f }, bodyColor });
						vertex.append(sf::Vertex{ target + sf::Vector2f{  headLength, -headWidth / 2.f }, bodyColor });
					}
					else
					{
						vertex.append(sf::Vertex{ target + sf::Vector2f{  -headLength,  headWidth / 2.f }, bodyColor });
						vertex.append(sf::Vertex{ target + sf::Vector2f{  -headLength, -headWidth / 2.f }, bodyColor });
					}
				}
			}
		}
	};
}
