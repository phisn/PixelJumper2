#pragma once

#include "ActiveUserContainer.h"
#include "ActiveHostContainer.h"

#include "Common/Common.h"
#include "NetCore/message/OperatorClassicSimulatorMessage.h"
#include "NetCore/RequestHandler.h"

namespace Operator::Net
{
	/*
		no meed for classiccommonrequesthandler or
		similar to allow user requesting of data because
		all requests should come though classic servers
		to minimize the request offhead for operator

		add the request to retrive user data
	*/
	class ClassicHostRequestHandler
		:
		public ::Net::RequestHandler
	{
	public:
		ClassicHostRequestHandler(
			UserID userID,
			SteamNetworkingIPAddr address)
			:
			userID(userID),
			hostContainer(userID)
		{
			hostContainer.host.address = address;
			hostContainer.maxPlayers = 100;
		}

		void update() override
		{
		}

		bool onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case ::Net::Client::OperatorClassicHostID::ClientData:
				if (::Net::Client::ClassicRequestClientDataMessage message; loadMessage(messageID, &message, pipe))
				{
					onRequestClientData(message);
				}

				return true;

			}

			return false;
		}

	private:
		const UserID userID;
		ClassicHostContainer hostContainer;

		void onRequestClientData(const ::Net::Client::ClassicRequestClientDataMessage& request)
		{
			::Net::Host::ClassicRequestClientDataMessage message;

			message.username = "username";
			message.resource.unlockedRepresentations = { 0 };
			message.resource.unlockedWorlds = { 0xdd0bb0dd };

			access->sendMessage(
				::Net::Host::OperatorClassicHostID::ClientData,
				&message);
		}
	};
}
