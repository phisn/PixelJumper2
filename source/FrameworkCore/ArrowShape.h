#pragma once

#include <SFML/Graphics.hpp>

namespace Framework
{
	/*
		  3 +\
	1	  2	| \
	+--------  \  Destination
  Source        > 4       -
	+--------  /      - # |
	0      6| /       |  headRatio
		   5+/        -
	
	|-------|
	bodyLength

	-------->
	bodyVector

	        |---|
			headLength

	------------>
	destination
	*/

	class ArrowShape
		:
		public sf::Drawable
	{
	public:
		ArrowShape(float width, float headLength, float ratioHead)
			:
			vertex(sf::PrimitiveType::TriangleStrip, 8),
			width(width),
			headLength(headLength),
			ratioHead(ratioHead)
		{
		}

		void setColor(sf::Color color)
		{
			for (int i = 0; i < 8; ++i)
				vertex[i].color = color;
		}

		void setSource(float x, float y)
		{
			setSource({ x, y });
		}

		void setSource(sf::Vector2f source)
		{
			this->source = source;
			needsUpdate = true;
		}

		void setDestination(float x, float y)
		{
			setDestination({ x, y });
		}

		void setDestination(sf::Vector2f destination)
		{
			this->destination = destination;
			needsUpdate = true;
		}

	private:
		float width, headLength, ratioHead;
		
		sf::Vector2f source, destination;
		bool needsUpdate = true;

		mutable sf::VertexArray vertex;

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override
		{
			if (needsUpdate)
				construct();

			target.draw(vertex, states);
		}

		void construct() const
		{
			float bodyWidth = (1 - ratioHead) * width;
			float headWidth = ratioHead * width;

			sf::Vector2f direction = destination - source;
			float directionAngle = atan2(direction.y, direction.x);

			float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
			float bodyLength = length - headLength;

			float p0_direction_x = sin(directionAngle);
			float p0_direction_y = -cos(directionAngle);

			float p0_direction_scale = bodyWidth / 2.f;

			vertex[6].position =
			{
				source.x + p0_direction_x * p0_direction_scale,
				source.y + p0_direction_y * p0_direction_scale
			};

			float p1_direction_x = -sin(directionAngle);
			float p1_direction_y = cos(directionAngle);

			float p1_direction_scale = bodyWidth / 2.f;

			vertex[5].position =
			{
				source.x + p1_direction_x * p1_direction_scale,
				source.y + p1_direction_y * p1_direction_scale
			};

			float direction_to_bodyVector_scale = bodyLength /
				sqrtf(direction.x * direction.x + direction.y * direction.y);

			sf::Vector2f bodyVector = direction * direction_to_bodyVector_scale;

			vertex[4].position = vertex[5].position + bodyVector;
			vertex[0].position =
			{
				vertex[4].position.x + p1_direction_x * headWidth,
				vertex[4].position.y + p1_direction_y * headWidth
			};

			vertex[1].position = destination;
			vertex[3].position = vertex[6].position + bodyVector;

			vertex[7] = vertex[3];

			vertex[2].position =
			{
				vertex[3].position.x + p0_direction_x * headWidth,
				vertex[3].position.y + p0_direction_y * headWidth
			};
		}
	};
}