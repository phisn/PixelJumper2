#pragma once

#include <Client/source/game/net/FrameStatus.h>
#include <Client/source/game/net/SimulatorAuthenticationMessage.h>

namespace Game::Net::Client
{
	struct ClassicSelectionMessageID
	{
		enum
		{
			_Begin = AuthenticationMessageID::_Offset - 1,

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
	};

	typedef TrivialNetworkMessage<PrepareSimulationMessageContent> PrepareSimulationMessage;
}

namespace Game::Net::Host
{
	struct ClassicSelectionMessageID
	{
		enum
		{
			_Begin = AuthenticationMessageID::_Offset - 1,

			AcceptSimulationRequest,
			RejectSimulationRequest,

			_Offset
		};
	};

	struct RejectSimulationRequestMessageContent
	{
		enum Reason
		{
			SimulationAlreadyRunning,
			InvalidWorldID,
			SimulationRunFailed

		} reason;
	};

	typedef TrivialNetworkMessage<RejectSimulationRequestMessageContent> RejectSimulationRequestMessage;
}
