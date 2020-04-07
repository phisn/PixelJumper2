#pragma once

#include "SimulatorAuthenticationMessage.h"
#include "SimulatorSettings.h"

namespace Net::Client
{
	struct ClassicSessionMessageID
	{
		enum
		{
			_Begin = AuthenticationMessageID::_Offset - 1,
			_Offset
		};
	};
}

namespace Net::Host
{
	struct ClassicSessionMessageID
	{
		enum
		{
			_Begin = AuthenticationMessageID::_Offset - 1,

			InitializeSession,

			AddPlayer,
			RemovePlayer,

			_Offset
		};
	};

	namespace ClassicSessionMessage
	{
		struct AddPlayer
			:
			public NetworkMessage
		{
			Resource::PlayerResource* player = NULL;

			bool load(Resource::ReadPipe* const pipe) override
			{
				return player->make(pipe);
			}

			bool save(Resource::WritePipe* const pipe) override
			{
				return player->save(pipe);
			}
		};

		struct RemovePlayerContent
		{
			Operator::UserID userID;
		};

		typedef TrivialNetworkMessage<RemovePlayerContent> RemovePlayer;

		struct InitializeSession
			:
			public NetworkMessage
		{
			struct Content
			{
				Game::SimulatorSettings settings;

			} content;

			std::vector<Resource::PlayerResource*> players;

			bool load(Resource::ReadPipe* const pipe) override
			{
				sf::Uint32 playerCount;
				if (!pipe->readValue(&playerCount))
				{
					return false;
				}

				players.resize(playerCount);
				for (Resource::PlayerResource*& player : players)
				{
					player = new Resource::PlayerResource;

					if (!player->make(pipe))
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
}
