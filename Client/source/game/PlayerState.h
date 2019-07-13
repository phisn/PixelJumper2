#pragma once

#include <Client/source/game/GameState.h>
#include <Client/source/resource/WorldResource.h>

#include <SFML/System.hpp>

#pragma pack(push, 1)

namespace Game
{
	typedef sf::Uint32 PlayerId;

	enum class Direction
	{
		Up,
		Left,
		Down,
		Right
	};

	struct IsOnGround
	{
		bool up, left, down, right;
	};

	inline bool operator==(const IsOnGround& iog1, const IsOnGround& iog2)
	{
		return iog1.up == iog2.up &&
			iog1.left == iog2.left &&
			iog1.down == iog2.down &&
			iog1.right == iog2.right;
	}

	inline bool operator!=(const IsOnGround& iog1, const IsOnGround& iog2)
	{
		return !(iog1 == iog2);
	}

	struct PlayerStateProperties
	{
		sf::Vector2f position;
		sf::Vector2f movement;

		bool hasForceLeft;
		bool hasForceRight; // TODO: replace

		float speed = 5;
		float weight = 10;
		float friction = 1;

		IsOnGround isOnGround;
		sf::Vector2f respawnPoint;

		Resource::WorldId world;

		struct
		{
			float rotation;

			bool useCustomPosition;
			sf::Vector2f position;

			sf::Vector2f size;

		} View;
	};

	class PlayerState
		:
		public GameState
	{
		PlayerStateProperties properties = { };

	public:
		bool writeState(
			Resource::WritePipe* const writePipe) override
		{
			return writePipe->writeValue(&properties);
		}

		bool readState(
			Resource::ReadPipe* const readPipe) override
		{
			return readPipe->readValue(&properties);
		}

		const PlayerStateProperties* readProperties() const
		{
			return &properties;
		}

		Property<sf::Vector2f> position{ properties.position };
		Property<sf::Vector2f> movement{ properties.movement };

		Property<bool> hasForceLeft{ properties.hasForceLeft };
		Property<bool> hasForceRight{ properties.hasForceRight };

		Property<float> speed{ properties.speed };
		Property<float> weight{ properties.weight };
		Property<float> friction{ properties.friction };

		Property<IsOnGround> isOnGround{ properties.isOnGround };
		Property<sf::Vector2f> respawnPoint{ properties.respawnPoint };

		Property<Resource::WorldId> worldId{ properties.world };

		//// View
		Property<float> view_rotation{ properties.View.rotation };

		// should set position before useCustomPosition
		Property<bool> view_useCustomPosition{ properties.View.useCustomPosition };
		Property<sf::Vector2f> view_position{ properties.View.position };

		Property<sf::Vector2f> view_size{ properties.View.size };
	};
}

#pragma pack(pop)
