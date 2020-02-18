#pragma once

#include <Client/source/game/net/UserAuthHostMessage.h>

namespace Game::Net::Host
{
	struct ClassicalConnectionMessageID
	{
		enum
		{
			_Begin = AuthenticationMessageID::_Offset - 1,

			AcceptSimulationRequest,
			RejectSimulationRequest,

			// adds or removes player from 
			// client view
			RemovePlayer,
			AddPlayer,

			// load other player movement
			PlayerMovement,

			// used to adjust client game
			// speed acording to connection
			// slow connections should have
			// higher delays and to achive
			// that a speeddown at the client
			// is used to cause a "falling
			// behind"
			SpeedUp,
			SpeedDown,

			// measure tick delay between client
			// and server
			Ping,

			// upload world resource to client
			PushResource,

			// sync player and world properties
			PrepareSync,
			MakeSync,

			// remove client from server
			Disconnect,

			_Offset
		};
	};

	struct RejectSimulationRequestMessageContent
	{
		enum Reason
		{
			SimulationAlreadyRunning,
			InvalidWorldID

		} reason;
	};

	typedef TrivialNetworkMessage<RejectSimulationRequestMessageContent> RejectSimulationRequestMessage;
}
