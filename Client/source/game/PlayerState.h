#pragma once

#include <Client/source/game/GameState.h>

#include <SFML/System.hpp>

namespace Game
{
	typedef sf::Uint32 PlayerId;

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
