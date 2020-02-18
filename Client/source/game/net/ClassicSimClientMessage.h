#pragma once

namespace Game::Net::Client
{
	struct ClassicSimMessageID
	{
		enum
		{
			_Begin = AuthenticationMessageID::_Offset - 1,

			// request simulation informations
			QuerySimulation,

			// request simulation start with
			// starting informations
			PrepareSimulation,

			// request a sync instead of 
			// waiting for the next one
			RequestSync,

			// accept a server side forced
			// sync
			AcceptSync,

			PushMovement,

			_Offset
		};
	};

	struct QuarySimulationMessage
	{
	};

	struct OpenSimulationMessageContent
	{
		Resource::WorldId world;
	};

	typedef TrivialNetworkMessage<OpenSimulationMessageContent> PrepareSimulationMessage;

	struct RequestSyncMessage
	{
	};

	struct AcceptSyncMessageContent
	{
		sf::Uint64 tickCount;
	};

	typedef TrivialNetworkMessage<AcceptSyncMessageContent> AcceptSyncMessage;
}
