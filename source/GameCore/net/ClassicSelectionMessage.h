#pragma once

#include "FrameStatus.h"
#include "ClassicCommonMessage.h"

#include "ResourceCore/ClassicPlayerResource.h"

namespace Net::Client
{
	struct ClassicSelectionMessageID
	{
		enum
		{
			_Begin = ClassicCommonMessageID::_Offset - 1,

			// request simulation informations
			QuerySimulation,

			// request simulation start with
			// starting informations
			PrepareSimulation,

			_Offset
		};
	};

	struct PrepareSimulationMessageContent
	{
		Resource::WorldId world;
		Resource::RepresentationID representationID;
	};

	typedef TrivialNetworkMessage<PrepareSimulationMessageContent> PrepareSimulationMessage;
}

namespace Net::Host
{
	struct ClassicSelectionMessageID
	{
		enum
		{
			_Begin = ClassicCommonMessageID::_Offset - 1,

			AcceptSimulationRequest,
			RejectSimulationRequest,

			_Offset
		};
	};

	struct AcceptSimulationRequestMessage
		:
		public NetworkMessage
	{
		// selected world plus surrounding
		std::vector<Resource::World*> initialWorlds;

		bool load(Resource::ReadPipe* const pipe) override
		{
			sf::Uint8 count;
			if (!pipe->readValue(&count))
			{
				return false;
			}

			initialWorlds.resize(count);

			for (int i = 0; i < initialWorlds.size(); ++i)
			{
				initialWorlds[i] = new Resource::World();

				if (!initialWorlds[i]->make(pipe))
					return false;
			}

			return true;
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			sf::Uint8 count = initialWorlds.size();
			if (!pipe->writeValue(&count))
			{
				return false;
			}

			for (Resource::World* const world : initialWorlds)
				if (!world->save(pipe))
				{
					return false;
				}

			return true;
		}
	};

	struct RejectSimulationRequestMessageContent
	{
		enum Reason
		{
			SimulationAlreadyRunning,
			InvalidWorldID,
			InvalidRepresentationID,
			InternalResourceMissing,
			SimulationRunFailed

		} reason;
	};

	typedef TrivialNetworkMessage<RejectSimulationRequestMessageContent> RejectSimulationRequestMessage;
}
