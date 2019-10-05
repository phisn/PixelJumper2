#pragma once

#include <Client/source/game/GameProperty.h>
#include <Client/source/game/GameState.h>
#include <Client/source/game/WorldInformation.h>

#include <SFML/Graphics.hpp>

namespace Game
{
	struct WorldState
	{
		sf::Uint16 playerCount;

		sf::Vector2f gravity;
		float airResistance;

		sf::Uint32 milisecondsPassed;
	};

	class WorldProperties
		:
		public GameState
	{
	public:
		WorldProperties()
		{
			playerCount = 0;
			milisecondsPassed = 0;
		}

		ReadOnlyProperty<Property<sf::Uint16>, WorldProperties> playerCount;

		DefinedProperty<sf::Vector2f> gravity{ Definition::gravity };
		DefinedProperty<float> airResistance{ Definition::air_resistance };

		Property<sf::Uint32> milisecondsPassed;

		void loadDefault(const WorldInformation info)
		{
			airResistance = info.defaultWorldProperties.airResistance;
			gravity = info.defaultWorldProperties.gravity;

			update();
		}

		void update()
		{
			playerCount.update();
			gravity.update();
			airResistance.update();
			milisecondsPassed.update();
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

			playerCount = rpp.playerCount;
			gravity = rpp.gravity;
			airResistance = rpp.airResistance;
			milisecondsPassed = rpp.milisecondsPassed;

			update(); // TODO: ?

			return true;
		}

		WorldState createState() const
		{
			WorldState rpp;

			rpp.playerCount = playerCount;
			rpp.gravity = gravity;
			rpp.airResistance = airResistance;
			rpp.milisecondsPassed = milisecondsPassed;

			return rpp;
		}

		void setPlayerCountValue(const sf::Uint16 value)
		{
			playerCount = value;
		}
	};
}
