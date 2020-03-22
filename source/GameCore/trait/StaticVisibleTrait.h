#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include <vector>

namespace Game
{
	struct StaticVisibleTrait
	{
		sf::Color color;

		sf::Vector2f position;
		sf::Vector2f size;
	};

	class StaticVisibleTraitContainer
	{
	public:
		void pushStaticVisibleTrait(const StaticVisibleTrait& staticVisible)
		{
			staticVisibles.push_back(staticVisible);
		}

		const std::vector<StaticVisibleTrait>& getStaticVisibleTrait() const
		{
			return staticVisibles;
		}

	protected:
		std::vector<StaticVisibleTrait> staticVisibles;
	};
}