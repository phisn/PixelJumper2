#pragma once

#include <SFML/Graphics.hpp>

namespace Framework
{
	/*
		  3 +\
	1	  2	| \
	+--------  \
	|           > 4
	+--------  /
	0      6| /
		   5+/
	
	*/

	class ArrowShape
		:
		public sf::Drawable
	{
	public:
		ArrowShape()
			:
			vertex(sf::PrimitiveType::Points, 7)
		{
		}

		void setSource(sf::Vector2f source)
		{
			this->source = source;
			needsUpdate = true;
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
			float bodyWidth = (1 - ratioHead) * width;
			float headWidth = ratioHead * width;

			sf::Vector2f direction = destination - source;
			float length = sqrtf(direction.x * direction.x + direction.y * direction.y);

			float bodyLength = length - headLength;

			float p0_direction_x = sin(direction.x);
			float p0_direction_y = -cos(direction.y);

			float p0_direction_scale = bodyWidth / 2.f;
			
			vertex[0].position =
			{
				p0_direction_x * p0_direction_scale,
				p0_direction_y * p0_direction_scale
			};

			vertex[7] = vertex[0];

			float p1_direction_x = -sin(direction.x);
			float p1_direction_y = cos(direction.y);

			float p1_direction_scale = bodyWidth / 2.f;

			vertex[1].position =
			{
				p1_direction_x * p1_direction_scale,
				p1_direction_y * p1_direction_scale
			};

			float direction_to_bodyVector_scale = bodyLength / 
				sqrtf(direction.x * direction.x + direction.y * direction.y);

			sf::Vector2f bodyVector = direction * direction_to_bodyVector_scale;

			vertex[2].position = vertex[1].position + bodyVector;
			vertex[3].position =
			{
				vertex[2].position.x + p0_direction_x * headWidth,
				vertex[2].position.y + p0_direction_y * headWidth
			};

			vertex[4].position = destination;
			vertex[6].position = vertex[0].position + bodyVector;

			vertex[5].position = 
			{
				vertex[6].position.x + p1_direction_x * headWidth,
				vertex[6].position.y + p1_direction_y * headWidth
			};
		}
	};
}