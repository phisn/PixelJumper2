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
		void onMovementChange(const sf::Vector2f old)
		{
		}

		void onPositionChange(const sf::Vector2f old)
		{
		}

		void onRespawnPositionChange(const sf::Vector2f old)
		{
		}

		void onIsOnGroundChange(const bool old)
		{
		}

	public:
		PlayerState()
			:
			movement(StateProperty<sf::Vector2f, PlayerState>(onMovementChange)),
			position(StateProperty<sf::Vector2f, PlayerState>(onPositionChange)),
			respawnPosition(StateProperty<sf::Vector2f, PlayerState>(onRespawnPositionChange)),
			isOnGround(StateProperty<bool, PlayerState>(onIsOnGroundChange))
		{
		}

		float speed;
		
		StateProperty<sf::Vector2f, PlayerState> movement;
		StateProperty<sf::Vector2f, PlayerState> position;
		StateProperty<sf::Vector2f, PlayerState> respawnPosition;
		StateProperty<bool, PlayerState> isOnGround;

		sf::Uint32 currentWorldId;

	private:
		bool writeState(
			Resource::WritePipe* const writePipe) override
		{
			if (!writePipe->writeValue(&movement.getValue()))
			{
				return false;
			} // ...

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
