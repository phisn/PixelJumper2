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
			InitiateClassicUser,
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

			InitializeSessionMessage,

			// used when something failed at server side
			// that makes continuation of this session impssible
			InterruptSession,

			AddPlayerMessage,
			RemovePlayerMessage,

			_Offset
		};
	};

	namespace ClassicSession
	{
		enum class InterruptSessionReason
		{
			RestoreOperatorStateFailed
		};

		struct InterruptSessionMessageContent
		{
			InterruptSessionReason reason;
		};

		typedef TrivialNetworkMessage<InterruptSessionMessageContent> InterruptSessionMessage;

		struct AddPlayerMessage
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

		struct RemovePlayerMessageContent
		{
			Operator::UserID userID;
		};

		typedef TrivialNetworkMessage<RemovePlayerMessageContent> RemovePlayerMessage;

		struct InitializeSessionMessage
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
