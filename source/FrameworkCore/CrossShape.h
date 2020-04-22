#pragma once

#include <SFML/Graphics.hpp>

namespace Framework
{
	class CrossShape
		:
		public sf::Drawable
	{
	public:
		// Make aviable for testing
		static void Construct(
			sf::Vertex* const vertices,
			const float consumption,
			const sf::Vector2f size)
		{
			/*      sl
				/---\
				   1+---+2
					|   |
			  11+---+0 3+---+4    \
				|           |     |
			  10+---+9 6+---+5    / cl
					|   |
				   8+---+7
			*/

			const sf::Vector2f sl = 0.5f * size * (1 - consumption);
			const sf::Vector2f cl = size * consumption;

			vertices[0].position = { sl.x, sl.y };						// 0000
			vertices[1].position = { sl.x, 0.f };						// 0001

			vertices[2].position = { sl.x + cl.x, 0.f };				// 0010
			vertices[3].position = { sl.x + cl.x, sl.y };				// 0011

			vertices[4].position = { sl.x * 2 + cl.x, sl.y };			// 0100
			vertices[5].position = { sl.x * 2 + cl.x, sl.y + cl.y };	// 0101

			vertices[6].position = { sl.x + cl.x, sl.y + cl.y };		// 0110
			vertices[7].position = { sl.x + cl.x, sl.y * 2 + cl.y };	// 0111

			vertices[8].position = { sl.x, sl.y * 2 + cl.y };			// 1000
			vertices[9].position = { sl.x, sl.y + cl.y };				// 1001

			vertices[10].position = { 0.f, sl.y + cl.y };				// 1010
			vertices[11].position = { 0.f, sl.y };						// 1011
		}

		CrossShape()
		{
			update();
		}

		virtual void draw(
			sf::RenderTarget& target,
			sf::RenderStates states) const override
		{
			target.draw(vertices, 12, sf::PrimitiveType::TriangleFan, states);
		}

		sf::Color getColor() const
		{
			return color;
		}

		void setColor(sf::Color color)
		{
			for (int i = 0; i < 12; ++i)
			{
				vertices[i].color = color;
			}

			this->color = color;
		}

		sf::Vector2f getPosition() const
		{
			return position;
		}

		void setPosition(sf::Vector2f position)
		{
			for (int i = 0; i < 12; ++i)
			{
				vertices[i].position += position - this->position;
			}

			this->position = position;
		}

		sf::Vector2f getSize() const
		{
			return position;
		}

		void setSize(sf::Vector2f size)
		{
			this->size = size;
			update();
		}

		float getConsumption() const
		{
			return consumption;
		}

		void setConsumption(float consumption)
		{
			this->consumption = consumption;
			update();
		}

	private:
		sf::Color color;
		sf::Vector2f position, size;

		// 1.f == rectangle, 0.f == nothing => smaller <=> thiner
		float consumption = 0.5f;

		void update()
		{
			Construct(vertices, consumption, size);

			for (int i = 0; i < 12; ++i)
			{
				vertices[i].position += position;
				vertices[i].color = color;
			}
		}

		sf::Vertex vertices[12];
	};

	/*
	class BufferedCrossShape
		:
		public CrossShape
	{
	public:
		BufferedCrossShape()
			:
			buffer(
				sf::TriangleFan,
				sf::VertexBuffer::Usage::Dynamic),
			CrossShape()
		{
			color.addListener(
				[this](const sf::Color oldColor,
					const sf::Color newColor)
				{
					buffer.update((const sf::Vertex*) & vertices);
				});
			position.addListener(
				[this](const sf::Vector2f oldPosition,
					const sf::Vector2f newPosition)
				{
					buffer.update((const sf::Vertex*) & vertices);
				});
			size.addListener(
				[this](const sf::Vector2f oldSize,
					const sf::Vector2f newSize)
				{
					buffer.update((const sf::Vertex*) & vertices);
				});
			consumption.addListener(
				[this](const float oldConsumption,
					const float newConsumption)
				{
					buffer.update((const sf::Vertex*) & vertices);
				});

			buffer.create(12);
			buffer.update((const sf::Vertex*) &vertices);
		}

		void draw(
			sf::RenderTarget& target,
			sf::RenderStates states) const override
		{
			target.draw(buffer, states);
		}

	private:
		sf::VertexBuffer buffer;
	};
	*/
}
