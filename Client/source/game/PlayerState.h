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
		Left,
		Right
	};

	class PlayerState
		:
		public GameState
	{
	public:
		struct
		{
			float speed;

			sf::Vector2f movement;
			sf::Vector2f position;

			sf::Vector2f respawnPosition;

			bool isOnGround;

			sf::Uint32 currentWorldId;
		} Properties;

	private:
		bool writeState(
			Resource::WritePipe* const writePipe) override
		{
			if (!writePipe->writeValue(&Properties))
			{
				return false;
			}

			// ...

			return true;
		}

		bool readState(
			Resource::ReadPipe* const readPipe) override
		{
			if (!readPipe->readValue(&Properties))
			{
				return false;
			}

			// ...

			return true;
		}
	};
}

#pragma pack(pop)
