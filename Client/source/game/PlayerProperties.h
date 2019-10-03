#pragma 


#include <Client/source/game/GameDefinitions.h>
#include <Client/source/game/GameProperty.h>
#include <Client/source/game/GameState.h>
#include <Client/source/game/tiletrait/GameTileBase.h>
#include <Client/source/game/WorldInformation.h>

#include <Client/source/resource/PlayerResource.h>

#include <map>

namespace Game
{
	class TileDataStorage
		:
		public GameState
	{
	public:
		void update()
		{
			for (decltype(tileData)::const_reference data : tileData)
				data.second.update();
		}

		bool writeState(Resource::WritePipe * const writePipe) override
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
		Property<sf::Vector2f> position, movement;

		DefinedProperty<float> mass{ Definition::mass };
		DefinedProperty<float> inputForce{ Definition::input_force };
		Property<float> inputReduce;

		TileDataStorage tileDataStorage;

		Property<bool> viewFollow;
		Property<sf::FloatRect> viewWindow;
		Property<float> viewRotation; // % 360

		void loadDefault(const WorldInformation& info)
		{
			position = info.defaultPlayerProperties.beginPosition;
			movement = info.defaultPlayerProperties.beginMovement;

			mass = info.defaultPlayerProperties.mass;
			inputForce = info.defaultPlayerProperties.inputForce;
			inputReduce = info.defaultPlayerProperties.inputReduce;

			viewFollow = info.defaultPlayerProperties.viewFollow;
			viewWindow = info.defaultPlayerProperties.viewWindow;
			viewRotation = info.defaultPlayerProperties.viewRotation;

			update();
		}

		void update()
		{
			position.update();
			movement.update();

			mass.update();
			inputForce.update();
			inputReduce.update();

			viewFollow.update();
			viewWindow.update();
			viewRotation.update();

			tileDataStorage.update();
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
