#pragma once

#include "OperatorAuthenticationMessage.h"

#include <steam/steamnetworkingsockets.h>

namespace Net::Client
{
	/*
		ideas:
		-	change username
		-!	change password
	
	*/
	struct OperatorCommonMessageID
	{
		enum
		{
			_Begin = OperatorAuthenticationMessageID::_Offset - 1,

			RequestConnectionKey,

			// find classic host
			HostFindClassic,

			// resolves userid to ip address
			// if userid is currently hosting
			HostResolveUserID,

			// registers as classic host and
			// enables classic requests
			RegisterClassicHost,

			_Offset
		};
	};

	struct RequestConnectionKeyMessageContent
	{
		Operator::UserID userID;
	};

	typedef ::Net::TrivialNetworkMessage<RequestConnectionKeyMessageContent> RequestConnectionKeyMessage;

	struct RegisterClassicHostMessageContent
	{
		sf::Uint16 maxPlayers;
		sf::Uint16 port;
	};

	typedef ::Net::TrivialNetworkMessage<RegisterClassicHostMessageContent> RegisterClassicHostMessage;
}

namespace Operator::Net::Host
{
	struct OperatorCommonMessageID
	{
		enum
		{
			_Begin = OperatorAuthenticationMessageID::_Offset - 1,

			ConnectionKey,

			// currently is only caused by userid
			// not found
			ConnectionKeyFailed,

			RegisterClassicHostRejected,
			RegisterClassicHostAccepted,

			HostFindClassicRejected,
			HostFindClassic,

			HostResolveUserIDRejected,
			HostResolveUserID,

			_Offset
		};
	};

	struct ConnectionKeyMessageContent
	{
		ConnectionKey key;
	};

	typedef ::Net::TrivialNetworkMessage<ConnectionKeyMessageContent> ConnectionKeyMessage;

	struct HostFindClassicMessageContent
	{
		ConnectionKey key;
		UserID userID;
		SteamNetworkingIPAddr address;
	};

	typedef ::Net::TrivialNetworkMessage<HostFindClassicMessageContent> HostFindClassicMessage;

	struct HostFindClassicRejectedMessageContent
	{
		enum
		{
			InvalidUserMode,
			NoHostAvailable
			
		} type;
	};

	typedef ::Net::TrivialNetworkMessage<HostFindClassicRejectedMessageContent> HostFindClassicRejectedMessage;
}
