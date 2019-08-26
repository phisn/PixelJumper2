#pragma once

#include <Client/source/game/PlayerInformation.h>

#include <SFML/Graphics.hpp>

namespace Game
{

	class PlayerRepresentation
	{
	public:
		static PlayerRepresentation* Create(
			const PlayerInformation& information);

		virtual void update(const sf::Time time) = 0;
		virtual void draw(sf::RenderTarget* const target) const = 0;

		virtual void setPosition(const sf::Vector2f position) = 0;
		virtual void setSize(const sf::Vector2f size) = 0;
	};

	class CommonPlayerRepresentation
		:
		public PlayerRepresentation
	{
	public:
		CommonPlayerRepresentation(
			const sf::Color outerColor = sf::Color::Color::Red,
			const sf::Color innerColor = sf::Color::Color::Green)
		{
			shape.setOutlineColor(outerColor);
			shape.setFillColor(innerColor);
		}

		void update(const sf::Time time) override
		{
		}

		void draw(sf::RenderTarget* const target) const override
		{
			target->draw(shape);
		}

		virtual void setPosition(const sf::Vector2f position) override
		{
			shape.setPosition(position);
		}

		virtual void setSize(const sf::Vector2f size) override
		{
			const float outerFactor = 0.1f;

			shape.setSize(size * (1 - outerFactor * 2));
			shape.setOutlineThickness(
				(size.x < size.y ? size.x : size.y)
				* outerFactor);
		}

	private:
		sf::RectangleShape shape;
	};
}
