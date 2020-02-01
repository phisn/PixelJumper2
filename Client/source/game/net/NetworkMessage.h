#pragma once

#include <Client/source/resource/pipes/PipeBase.h>

namespace Game::Net
{
	struct NetworkMessage
	{
		virtual bool load(Resource::ReadPipe* const pipe) = 0;
		virtual bool save(Resource::WritePipe* const pipe) = 0;
	};

	struct CommonMessageID
	{
		enum
		{
			Error,
			_Offset
		};
	};

	namespace Client
	{
		struct AuthenticationMessageID
		{
			enum
			{
				_Begin = CommonMessageID::_Offset - 1,

				// request auth from the host
				Authenticate,
				_Offset
			};
		};
	}

	namespace Host
	{
		struct AuthenticationMessageID
		{
			enum
			{
				_Begin = CommonMessageID::_Offset - 1,

				// auth accepted and playerid
				// validated
				AcceptAuthentication,

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
}
