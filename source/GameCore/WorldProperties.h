#pragma once

#include "Common/Common.h"

#include "GameProperty.h"
#include "GameState.h"
#include "WorldInformation.h"

#include <SFML/Graphics.hpp>

namespace Game
{
	struct WorldState
	{
		sf::Uint16 playerCount;

		sf::Vector2f gravity;
		float airResistance;

		sf::Uint32 tickCount;
	};

	class WorldProperties
		:
		public GameState
	{
		friend class World;

	public:
		WorldProperties()
		{
			playerCount = 0;
			tickCount = 0;
		}

		ReadOnlyProperty<Property<sf::Uint16>, WorldProperties> playerCount;

		DefinedProperty<sf::Vector2f> gravity{ Shared::ValueDefinition::gravity };
		DefinedProperty<float> airResistance{ Shared::ValueDefinition::air_resistance };

		ReadOnlyProperty<Property<sf::Uint32>, WorldProperties> tickCount;

		void loadDefault(const WorldInformation info)
		{
			airResistance = info.defaultProperties.airResistance;
			gravity = info.defaultProperties.gravity;

			update();
		}

		void update()
		{
			playerCount.update();
			gravity.update();
			airResistance.update();
			tickCount.update();
		}

		bool writeState(Resource::WritePipe* const writePipe) override
		{
			WorldState rpp = createState();
			return writePipe->writeValue(&rpp);
		}

		bool readState(Resource::ReadPipe* const readPipe) override
		{
			WorldState rpp;

			if (!readPipe->readValue(&rpp))
			{
				return false;
			}

			if (playerCount != rpp.playerCount)
			{
				return false;
			}

			playerCount = rpp.playerCount;
			gravity = rpp.gravity;
			airResistance = rpp.airResistance;
			tickCount = rpp.tickCount;

			update(); // TODO: ?

			return true;
		}

		WorldState createState() const
		{
			WorldState rpp;

			rpp.playerCount = playerCount;
			rpp.gravity = gravity;
			rpp.airResistance = airResistance;
			rpp.tickCount = tickCount;

			return rpp;
		}

	private:
		void setPlayerCount(const sf::Uint16 value)
		{
			playerCount = value;
		}

		void setTickCount(const sf::Uint64 value)
		{
			tickCount = value;
		}

		void increaseTickCount()
		{
			++tickCount;
		}
	};
}
