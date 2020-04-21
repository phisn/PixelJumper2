#pragma once

#include "SimulatorAuthenticationMessage.h"
#include "SimulatorSettings.h"

#include "ResourceCore/WorldResource.h"

#include <cassert>

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

			InitializeSession,

			WorldUnlocked,
			RepresentationUnlocked,

			// used when something failed at server side
			// that makes continuation of this session impssible
			InterruptSession,

			AddPlayer,
			RemovePlayer,

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
				assert(player != NULL);

				return player->make(pipe);
			}

			bool save(Resource::WritePipe* const pipe) override
			{
				assert(player != NULL);

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

		struct WorldUnlockedMessage
			:
			public NetworkMessage
		{
			// has to be provided upfront
			Resource::World* world = NULL;

			bool load(Resource::ReadPipe* const pipe) override
			{
				assert(world != NULL);

				return world->make(pipe);
			}

			bool save(Resource::WritePipe* const pipe) override
			{
				assert(world != NULL);

				return world->save(pipe);
			}
		};

		struct RepresentationUnlockedMessageContent
		{
			Resource::RepresentationID representationID;
		};

		typedef TrivialNetworkMessage<RepresentationUnlockedMessageContent> RepresentationUnlockedMessage;
	}
}
