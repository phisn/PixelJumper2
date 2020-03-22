#pragma once

#include "Common/Common.h"

#include "GameProperty.h"
#include "GameState.h"
#include "WorldInformation.h"

#include "ResourceCore/PlayerResource.h"

#include <map>

namespace Game
{
	struct PlayerInput
	{
	};


	// add worldid to identity
	class TileDataStorage
		:
		public GameState
	{
	public:
		bool update()
		{
			bool hasUpdated = false;

			for (decltype(tileData)::const_reference data : tileData)
				if (data.second.update())
				{
					hasUpdated = true;
				}

			return hasUpdated;
		}

		bool writeState(Resource::WritePipe* const writePipe) override
		{
			for (decltype(tileData)::const_reference data : tileData)
				if (!data.second->writeState(writePipe))
				{
					return false;
				}

			return true;
		}

		bool readState(Resource::ReadPipe * const readPipe) override
		{
			for (decltype(tileData)::const_reference data : tileData)
				if (!data.second->readState(readPipe))
				{
					return false;
				}

			return true;
		}

		template <typename T>
		T* readAs(const TileIdentity identity) const
		{
			return dynamic_cast<T*>(tileData[identity].getValue());
		}

		std::map<TileIdentity, Property<GameState*>> tileData;
	};

	struct PlayerState
	{
		sf::Vector2f spawn;
		sf::Vector2f position, movement;
		float mass, inputForce, inputReduce;

		bool viewFollow;
		sf::FloatRect viewWindow;
		float viewRotation;
	};

	class PlayerProperties
		:
		public GameState
	{
	public:
		// Property<Resource::WorldId> worldId;

		Property<sf::Vector2f> spawn;
		Property<sf::Vector2f> position, movement;

		DefinedProperty<float> mass{ Shared::ValueDefinition::mass };
		DefinedProperty<float> inputForce{ Shared::ValueDefinition::input_force };
		Property<float> inputReduce;

		TileDataStorage tileDataStorage;

		Property<bool> viewFollow;
		Property<sf::FloatRect> viewWindow;
		Property<float> viewRotation; // % 360

		void loadDefault(const WorldInformation& info)
		{
			spawn = info.defaultPlayerProperties.position;
			position = info.defaultPlayerProperties.position;
			movement = info.defaultPlayerProperties.movement;

			mass = info.defaultPlayerProperties.mass;
			inputForce = info.defaultPlayerProperties.inputForce;
			inputReduce = info.defaultPlayerProperties.inputReduce;

			viewFollow = info.defaultPlayerProperties.viewFollow;
			viewWindow = info.defaultPlayerProperties.viewWindow;
			viewRotation = info.defaultPlayerProperties.viewRotation;

			update();
		}

		bool update()
		{
			// has to be bitand because
			// normal does omit remaining updates
			return spawn.update()
				| position.update()
				| movement.update()

				| mass.update()
				| inputForce.update()
				| inputReduce.update()

				| viewFollow.update()
				| viewWindow.update()
				| viewRotation.update()

				| tileDataStorage.update();
		}

		bool writeState(Resource::WritePipe* const writePipe) override
		{
			PlayerState rpp = createState();
			return writePipe->writeValue(&rpp)
				&& tileDataStorage.writeState(writePipe);
		}

		bool readState(Resource::ReadPipe* const readPipe) override
		{
			PlayerState rpp;

			if (!readPipe->readValue(&rpp) &&
				!tileDataStorage.readState(readPipe))
			{
				return false;
			}

			spawn.setValue(rpp.spawn);
			position.setValue(rpp.position);
			movement.setValue(rpp.movement);

			mass.setValue(rpp.mass);
			inputForce.setValue(rpp.inputForce);
			inputReduce.setValue(rpp.inputReduce);

			viewFollow.setValue(rpp.viewFollow);
			viewWindow.setValue(rpp.viewWindow);
			viewRotation.setValue(rpp.viewRotation);

			update(); // TODO: ?

			return true;
		}

		PlayerState createState() const
		{
			PlayerState rpp;

			rpp.spawn = spawn;
			rpp.position = position;
			rpp.movement = movement;

			rpp.mass = mass;
			rpp.inputForce = inputForce;
			rpp.inputReduce = inputReduce;

			rpp.viewFollow = viewFollow;
			rpp.viewWindow = viewWindow;
			rpp.viewRotation = viewRotation;

			return rpp;
		}
	};
}
