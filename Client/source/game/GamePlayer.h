#pragma once

#include <Client/source/game/CollisionEngine.h>
#include <Client/source/game/GameProperty.h>
#include <Client/source/game/GameState.h>
#include <Client/source/game/PlayerRepresentation.h>

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

	struct RawPlayerProperties
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
			RawPlayerProperties rpp = createRaw();
			return writePipe->writeValue(&rpp)
				&& tileDataStorage.writeState(writePipe);
		}

		bool readState(Resource::ReadPipe* const readPipe) override
		{
			RawPlayerProperties rpp;

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

		RawPlayerProperties createRaw() const
		{
			RawPlayerProperties rpp;

			rpp.inputReducedFriction = inputReducedFriction;

			rpp.position = position;
			rpp.movement = movement;

			rpp.speed = speed;
			rpp.weight = weight;
			rpp.friction = friction;

			return rpp;
		}
	};

	class CollidableTile;
	class CollisionContainer
	{
	public:
		bool has(
			const CollisionEngine::CollisionInfo::Type type) const
		{
			return !!infos[type];
		}

		CollidableTile* operator[](
			const CollisionEngine::CollisionInfo::Type type) const
		{
			return infos[type];
		}

		void clear()
		{
			infos[0] = { };
			infos[1] = { };
			infos[2] = { };
			infos[3] = { };
		}

		void setCollision(
			const CollisionEngine::CollisionInfo::Type type,
			CollidableTile* const tile)
		{
			infos[type] = tile;
		}

	private:
		CollidableTile* infos[4];
	};

	class PlayerBase
	{
	public:	
		PlayerBase(const PlayerInformation information)
			:
			information(information),
			representation(PlayerRepresentation::Create(information))
		{
			properties.position.addListener(
				[this](const sf::Vector2f oldPosition,
					   const sf::Vector2f newPosition)
				{
					representation->setPosition(newPosition);
				});

			representation->setSize({ 1.f , 1.f });
		}

		~PlayerBase()
		{
			delete representation;
		}

		virtual void onLogic(const sf::Time time)
		{
			representation->update(time);
		}

		virtual void onDraw(sf::RenderTarget* const target)
		{
			representation->draw(target);
		}

		PlayerProperties& getProperties()
		{
			return properties;
		}

		const PlayerInformation& getInformation() const
		{
			return information;
		}

	protected:
		PlayerProperties properties;
		CollisionContainer collisionContainer; // expose

	private:
		PlayerRepresentation* const representation;
		const PlayerInformation information;
	};
}