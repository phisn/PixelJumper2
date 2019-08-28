#pragma 

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
		bool inputReducedFriction;
		sf::Vector2f position, movement;
		float speed, weight, friction;
	};

	class PlayerProperties
		:
		public GameState
	{
	public:
		Property<bool> inputReducedFriction;
		Property<sf::Vector2f> position, movement;
		Property<float> speed, weight, friction;

		TileDataStorage tileDataStorage;

		void loadDefault(const WorldInformation& info)
		{
			position = info.defaultPlayerProperties.beginPosition;
			movement = info.defaultPlayerProperties.beginMovement;

			speed = info.defaultPlayerProperties.speed;
			weight = info.defaultPlayerProperties.weight;
			friction = info.defaultPlayerProperties.friction;

			update();
		}

		void update()
		{
			inputReducedFriction.update();

			position.update();
			movement.update();

			speed.update();
			weight.update();
			friction.update();

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

			inputReducedFriction.setValue(rpp.inputReducedFriction);

			position.setValue(rpp.position);
			movement.setValue(rpp.movement);

			speed.setValue(rpp.speed);
			weight.setValue(rpp.weight);
			friction.setValue(rpp.friction);

			update(); // TODO: ?

			return true;
		}

		PlayerState createState() const
		{
			PlayerState rpp;

			rpp.inputReducedFriction = inputReducedFriction;

			rpp.position = position;
			rpp.movement = movement;

			rpp.speed = speed;
			rpp.weight = weight;
			rpp.friction = friction;

			return rpp;
		}
	};
}
