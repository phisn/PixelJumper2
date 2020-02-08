#pragma once

#include <Client/source/game/net/NetworkMessage.h>
#include <Client/source/resource/PlayerResource.h>

#include <Operator/source/Common.h>

namespace Operator::Net::Host
{
	struct AuthMessageID
	{
		enum
		{
			_Begin = Game::Net::CommonMessageID::_Offset - 1,

			AcceptRegistration,
			RejectRegistration,

			AcceptAuthentication,
			RejectAuthentication,
			
			_Offset
		};
	};

	struct AcceptAuthenticationMessageContent
	{
		AuthenticationToken token;
	};

	typedef Game::Net::TrivialNetworkMessage<AcceptAuthenticationMessageContent> AcceptAuthenticationMessage;

	struct AcceptRegistrationMessageContent
	{
		AuthenticationToken token;
		Resource::PlayerID playerID;
	};

	typedef Game::Net::TrivialNetworkMessage<AcceptRegistrationMessageContent> AcceptRegistrationMessage;

	struct RejectRegistrationMessageContent
	{
		enum Reason
		{
			UsernameUsed,
			KeyUsed,
			KeyInvalid,
			InternalError

		} reason;
	};

	typedef Game::Net::TrivialNetworkMessage<RejectRegistrationMessageContent> RejectRegistrationMessage;
}
