#pragma once

#include <Client/source/game/net/ServerMessage.h>

namespace Game::Net::Host
{
	/*
	enum class ServerMessage
	{
		Accept,
		LoadResource,
		Synchronize,

		Error,
		Disconnect
	};
	*/

	struct AuthenticationMessageID
	{
		enum
		{
			_Begin = CommonMessageID::_Offset - 1,

			// auth accepted and playerid
			// validated
			AcceptAuthentication,

			// invalid authentication structure
			// host was unable to retrive authentication
			// data
			InvalidAuthentication,

			// got not authentication in time
			Timeout,

			// auth token or salt is invalid
			InvalidToken,
			// playerid is invalid or does
			// not correspond to the token
			// ones
			InvalidPlayerID,

			// operator timed out on requesting
			// auth validation
			OperatorTimeout,
			// lost connection to operator and
			// auth is no longer valid
			OperatorDisconnect,

			_Offset
		};
	};

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
			// invalid content as unable to read the message
			// or completly invalid content
			InvalidSimulationRequestContent

		} reason;
	};

	typedef TrivialNetworkMessage<RejectSimulationRequestMessageContent> RejectSimulationRequestMessage;
}
