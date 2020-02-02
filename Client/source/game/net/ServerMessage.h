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

			LoadContext = AuthenticationMessageID::_Offset,

			_Offset
		};
	};
}
