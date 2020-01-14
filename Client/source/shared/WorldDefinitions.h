#pragma once

#include <SFML/Graphics.hpp>

namespace Shared
{
	struct WorldDefaultProperties
	{
		float airResistance;
		sf::Vector2f gravity;
	};

	struct PlayerDefaultProperties
	{
		sf::Vector2f movement;
		sf::Vector2f position;

		float mass;
		float inputForce;
		float inputReduce;

		bool viewFollow;
		sf::Vector2f viewWindow;
		float viewRotation;
	};
}