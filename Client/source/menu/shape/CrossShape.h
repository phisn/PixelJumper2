#pragma once

#include <Client/source/menu/MenuProperty.h>

#include <SFML/Graphics/Drawable.hpp>

namespace Menu
{
	class CrossShape
		:
		public sf::Drawable
	{
	public:
		CrossShape()
			:
			buffer(
				sf::TriangleFan,
				sf::VertexBuffer::Usage::Dynamic)
		{
			color.addListener(
				[this](const sf::Color oldColor,
					   const sf::Color newColor)
				{
					update();
				});
			position.addListener(
				[this](const sf::Vector2f oldPosition,
					   const sf::Vector2f newPosition)
				{
					update();
				});
			size.addListener(
				[this](const sf::Vector2f oldSize,
					   const sf::Vector2f newSize)
				{
					update();
				});
			consumption.addListener(
				[this](const float oldConsumption,
					   const float newConsumption)
				{
					update();
				});

			buffer.create(12);
			update();
		}

		Property<sf::Color> color{ sf::Color::White };
		Property<sf::Vector2f> position{ sf::Vector2f(0, 0) };
		Property<sf::Vector2f> size{ sf::Vector2f(0, 0) };
		Property<float> consumption{ 0.5f };

		void draw(
			sf::RenderTarget& target, 
			sf::RenderStates states) const override
		{
			target.draw(buffer, states);
		}

	private:
		sf::VertexBuffer buffer;

		void update()
		{
			sf::Vertex vertices[12];

			/*
				   1+---+2
					|   |
			  11+---+0 3+---+4    \
				|           |     |
			  10+---+9 6+---+5    / cl
					|   |
				   8+---+7
			*/

			const sf::Vector2f sl = 0.5f * size.getValue() * (1 - consumption.getValue());
			const sf::Vector2f cl = size.getValue() * consumption.getValue();

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


			for (int i = 0; i < 12; ++i)
			{
				vertices[i].position += position.getValue();
				vertices[i].color = color.getValue();
			}

			buffer.update((const sf::Vertex*) &vertices);
		}

		// 1.f == rectangle, 0.f == nothing => smaller <=> thiner
	};
}
