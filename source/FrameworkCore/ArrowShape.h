#pragma once

#include "FrameworkCore/FrameworkCore.h"

#include <SFML/Graphics.hpp>

namespace Framework
{
	/*
		  0 +\
	5	  4	| \
	+--------  \  Destination
  Source        > 1       -
	+--------7 /      - # |
	6      3| /       |  headRatio
		   2+/        -
	
	|-------|
	bodyLength

	-------->
	bodyVector

	        |---|
			headLength

	------------>
	destination
	*/

	enum class ArrowShapeMode
	{
		Default,
		Reversed,
		Doubled
	};

	class ArrowShape
		:
		public sf::Drawable
	{
	public:
		ArrowShape()
			:
			ArrowShape(6.0f, 15.0f, 0.6f)
		{
		}

		ArrowShape(float width, float headLength, float ratioHead)
			:
			vertex(sf::PrimitiveType::TriangleStrip, 10),
			width(width),
			headLength(headLength),
			ratioHead(ratioHead),
			mode(ArrowShapeMode::Doubled)
		{
			setColor(sf::Color::White);
		}

		void setMode(ArrowShapeMode mode)
		{
			if (mode != this->mode)
			{
				sf::Color color = vertex[0].color;

				if (mode == ArrowShapeMode::Doubled)
				{
					this->mode = mode;

					vertex.resize(10);
					setColor(color);
				}
				else if (this->mode == ArrowShapeMode::Doubled)
				{
					this->mode = mode;

					vertex.resize(8);
					setColor(color);
				}

				needsUpdate = true;
			}
		}

		void setColor(sf::Color color)
		{
			for (int i = 0; i < (mode == ArrowShapeMode::Doubled ? 10 : 8); ++i)
				vertex[i].color = color;
		}

		void setWidth(float width)
		{
			this->width = width;
			needsUpdate = true;
		}

		void setHeadLength(float headLength)
		{
			this->headLength = headLength;
			needsUpdate = true;
		}

		void setRatioHead(float ratioHead)
		{
			this->ratioHead = ratioHead;
			needsUpdate = true;
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

		bool contains(sf::Vector2f point) const
		{
			if (getGlobalBounds().contains(point))
			{
				if (Framework::HasRectPoint(point,
						vertex[6].position,
						vertex[5].position,
						vertex[4].position,
						vertex[3].position))
				{
					return true;
				}

				return Framework::HasTriPoint(point,
					vertex[0].position,
					vertex[1].position,
					vertex[2].position);
			}

			return false;
		}

		sf::Vector2f getSource() const
		{
			return source;
		}

		sf::Vector2f getDestination() const
		{
			return destination;
		}

		sf::Vector2f getPosition() const
		{
			return
			{
				std::min(destination.x, source.x),
				std::min(destination.y, source.y)
			};
		}

		sf::Vector2f getSize() const
		{
			return
			{
				std::abs(destination.x - source.x),
				std::abs(destination.y - source.y)
			};
		}

		sf::FloatRect getGlobalBounds() const
		{
			sf::Vector2f position = getPosition();
			sf::Vector2f size = getSize();

			return
			{
				position.x,
				position.y,
				size.x,
				size.y
			};
		}

	private:
		ArrowShapeMode mode = ArrowShapeMode::Default;

		float width, headLength, ratioHead;
		
		sf::Vector2f source, destination;
		bool needsUpdate = true;

		mutable sf::VertexArray vertex;

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override
		{
			if (needsUpdate)
				construct();
			
			if (source != destination)
				target.draw(vertex, states);
		}

		void construct() const
		{
			float bodyWidth = (1 - ratioHead) * width;
			float headWidth = ratioHead * width;

			sf::Vector2f direction = destination - source;

			if (mode == ArrowShapeMode::Reversed)
			{
				direction = -direction;
				vertex[1] = source;				
			}
			else
			{
				vertex[1] = destination;
			}

			float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
			float halfBodyLength = (length - headLength) / 2.f;

			float direction_to_halfBodyVector_scale = halfBodyLength /
				sqrtf(direction.x * direction.x + direction.y * direction.y);

			float directionAngle = atan2(direction.y, direction.x);

			float ptop_direction_x = -sin(directionAngle);
			float ptop_direction_y = cos(directionAngle);

			float ptop_direction_scale = bodyWidth / 2.f;

			float pbot_direction_x = sin(directionAngle);
			float pbot_direction_y = -cos(directionAngle);

			float pbot_direction_scale = bodyWidth / 2.f;

			sf::Vector2f middle = (destination + source) / 2.f;
			sf::Vector2f middleTop = middle + sf::Vector2f
			{
				ptop_direction_x * ptop_direction_scale,
				ptop_direction_y * ptop_direction_scale
			};

			sf::Vector2f middleBot = middle + sf::Vector2f
			{
				pbot_direction_x * pbot_direction_scale,
				pbot_direction_y * pbot_direction_scale
			};

			sf::Vector2f halfBodyVector = direction * direction_to_halfBodyVector_scale;

			vertex[4].position = middleTop + halfBodyVector;
			vertex[0].position = 
			{
				vertex[4].position.x + ptop_direction_x * headWidth,
				vertex[4].position.y + ptop_direction_y * headWidth
			};
			
			vertex[3].position = middleBot + halfBodyVector;

			vertex[2].position =
			{
				vertex[3].position.x + pbot_direction_x * headWidth,
				vertex[3].position.y + pbot_direction_y * headWidth
			};

			if (mode == ArrowShapeMode::Doubled)
			{
				vertex[6].position = middleTop - halfBodyVector;
				vertex[5].position = middleBot - halfBodyVector;

				vertex[7].position =
				{
					vertex[6].position.x + ptop_direction_x * headWidth,
					vertex[6].position.y + ptop_direction_y * headWidth
				};

				vertex[9].position =
				{
					vertex[5].position.x + pbot_direction_x * headWidth,
					vertex[5].position.y + pbot_direction_y * headWidth
				};

				vertex[8].position = source;
			}
			else
			{
				vertex[7].position = vertex[3].position;

				vertex[5].position = middleTop - direction / 2.f;
				vertex[6].position = middleBot - direction / 2.f;
			}
		}
	};
}