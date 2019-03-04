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
	public:
		struct _IsOnGround
		{
			bool up, left, down, right;
		};

		struct
		{
			sf::Vector2f position;
			sf::Vector2f movement;

			float speed;
			float weight;
			float drag; // todo rename

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
		} Properties;

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
