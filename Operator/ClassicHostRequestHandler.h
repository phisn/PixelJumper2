#pragma once

#include "ActiveUserContainer.h"
#include "net/ClassicSimulatorMessage.h"

#include "Common/Common.h"
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
		ClassicHostRequestHandler(const UserID userID)
			:
			userID(userID)
		{
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
			case Client::ClassicHostID::RequestClientData:
				if (Client::RequestClientDataMessage message; loadMessage(messageID, &message, pipe))
				{
					onRequestClientData(message);
				}

				return true;

			}

			return false;
		}

	private:
		const UserID userID;

		void onRequestClientData(const Client::RequestClientDataMessage& request)
		{
			if (UserContainer::GetUserMode(request.userID) != ActiveUserMode::Waiting)
			{
				Host::RequestClientDataFailedMessage message;
				message.type = message.InvalidUserMode;

				access->sendMessage(
					Host::ClassicHostID::RequestClientDataFailed,
					&message);
			}
			
			Host::RequestClientDataMessage message;

			message.username = "username";
			message.resource.unlockedRepresentations = { 0 };
			message.resource.unlockedWorlds = { 0x48f1da35 };

			access->sendMessage(
				Host::ClassicHostID::RequestClientData,
				&message);
		}
	};
}
