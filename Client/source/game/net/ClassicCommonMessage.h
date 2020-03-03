#pragma once

#include <Client/source/game/net/SimulatorAuthenticationMessage.h>

namespace Game::Net::Client
{
	struct ClassicCommonMessageID
	{
		enum
		{
			_Begin = AuthenticationMessageID::_Offset - 1,
			_Offset
		};
	};
}

namespace Game::Net::Host
{
	struct ClassicCommonMessageID
	{
		enum
		{
			_Begin = AuthenticationMessageID::_Offset - 1,

			InitializeClient,

			AddPlayer,
			RemovePlayer,

			_Offset
		};
	};


	struct InitializeClientMessage
		:
		public NetworkMessage
	{
		std::vector<Resource::PlayerResource*> players;

		bool load(Resource::ReadPipe* const pipe) override
		{
			sf::Uint32 playerCount;
			if (!pipe->readValue(&playerCount))
			{
				return false;
			}

			players.resize(playerCount);
			for (Resource::PlayerResource* const player : players)
				if (!player->make(pipe))
				{
					return false;
				}

			return true;
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			sf::Uint32 playerCount = players.size();
			if (!pipe->writeValue(&playerCount))
			{
				return false;
			}

			for (Resource::PlayerResource* const player : players)
				if (!player->save(pipe))
				{
					return false;
				}

			return true;
		}
	};
}
