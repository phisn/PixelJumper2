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

	struct PlayerStateProperties
	{
		struct _IsOnGround
		{
			bool up, left, down, right;
		};

		sf::Vector2f position;
		sf::Vector2f movement;

		bool hasForceLeft;
		bool hasForceRight; // TODO: replace

		float speed = 5;
		float weight = 10;
		float friction = 1;

		_IsOnGround isOnGround;
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

		PropertyWriter<sf::Vector2f> position{ properties.position };
		PropertyWriter<sf::Vector2f> movement{ properties.movement };

		PropertyWriter<bool> hasForceLeft{ properties.hasForceLeft };
		PropertyWriter<bool> hasForceRight{ properties.hasForceRight };

		PropertyWriter<float> speed{ properties.speed };
		PropertyWriter<float> weight{ properties.weight };
		PropertyWriter<float> friction{ properties.friction };

		PropertyWriter<PlayerStateProperties::_IsOnGround> isOnGround{ properties.isOnGround };
		PropertyWriter<sf::Vector2f> respawnPoint{ properties.respawnPoint };

		PropertyWriter<Resource::WorldId> worldId{ properties.world };

		//// View
		PropertyWriter<float> view_rotation{ properties.View.rotation };

		// should set position before useCustomPosition
		PropertyWriter<bool> view_useCustomPosition{ properties.View.useCustomPosition };
		PropertyWriter<sf::Vector2f> view_position{ properties.View.position };

		PropertyWriter<sf::Vector2f> view_size{ properties.View.size };
	};
}

#pragma pack(pop)
