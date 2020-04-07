#pragma once

#include "SFML/System.hpp"
#include "SFML/Graphics/Rect.hpp"

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
		sf::FloatRect viewWindow;
		float viewRotation;
	};

	namespace ValueDefinition
	{
		constexpr float input_force = 0.00005f;
		constexpr float mass = 1.28;

		constexpr float air_resistance = 0.99864f;
		constexpr float gravity = 0.000063f;

		constexpr float tile_density = 0.0247f;
		constexpr float tile_friction = 0.000027f;
		constexpr float tile_input = 1.7065f;
	}
}