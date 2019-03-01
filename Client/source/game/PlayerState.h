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

	class PlayerState
		:
		public GameState
	{
		friend class PlayerBase;

		struct
		{
			float speed;

			sf::Vector2f movement;
			sf::Vector2f position;
			sf::Vector2f respawnPosition;

			bool isOnGround;

			sf::Uint32 currentWorldId;

		} Properties;

	public:
		StateProperty<float> speed{ Properties.speed };
		
		StateProperty<sf::Vector2f> movement{ Properties.movement };
		StateProperty<sf::Vector2f> position{ Properties.position };
		StateProperty<sf::Vector2f> respawnPosition{ Properties.respawnPosition };
		StateProperty<bool> isOnGround{ Properties.isOnGround };

		StateProperty<sf::Uint32> currentWorldId{ Properties.currentWorldId };

	private:
		bool writeState(
			Resource::WritePipe* const writePipe) override
		{
			return writePipe->writeValue(&Properties);
		}

		bool readState(
			Resource::ReadPipe* const readPipe) override
		{
			return readPipe->readValue(&Properties);
		}
	};
}

#pragma pack(pop)
