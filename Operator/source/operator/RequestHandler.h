#pragma once

#include <Operator/source/net/RequestClientMessage.h>
#include <Operator/source/net/RequestOperatorMessage.h>

#include <Operator/source/operator/AuthenticationHandler.h>

namespace Operator::Net
{
	class RequestHandler
		:
		public AuthenticationHandler
	{
	public:
		AuthenticationHandler::AuthenticationHandler;

		void onThreatIdentified(
			const Device::Net::MessageID messageID,
			const wchar_t* const note,
			const Device::Net::ThreatLevel level) override
		{
		}
		
		void onClientAuthenticated() override
		{
			Log::Information(L"Client Authenticated");
		}

	private:
		void onClientAuthenticated()
		{

		}

		void onMessage(
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			if (status == Authenticating)
			{
				AuthenticationHandler::onMessage(messageID, pipe);
			}
			else
			{
				switch (messageID)
				{
				case Client::RequestMessageID::RequestConnectionKey:
				{
					Client::RequestConnectionKeyMessage message;

					if (loadCommonMessage(messageID, &message, pipe))
					{
						return;
					}

					onRequestConnnectionKey(message);

					break;
				}
				}
			}
		}

		void onRequestConnnectionKey(const Client::RequestConnectionKeyMessage& request)
		{
			ConnectionKeySource keySource;

			Database::ConditionResult result = Database::Interface::GetPlayerToken(
				keySource.token,
				request.userID);

			switch (result)
			{
			case Database::ConditionResult::NotFound:
				beginMessage(Host::RequestMessageID::ConnectionKeyFailed);
				sendMessage();

				return;
			case Database::ConditionResult::Error:
				beginMessage(Host::RequestMessageID::InternalError);
				sendMessage();

				return;
			}

			keySource.userID = request.userID;

			Host::ConnectionKeyMessage message;
			message.key.make(keySource);

			sendCommonMessage(
				Host::RequestMessageID::ConnectionKey,
				&message);
		}

		/*
		
		1. client requests to connect with host userID
		2. operator retrives host token for host userID
		3. operator creates hash [hostToken + clientUserID]
		4. operator sends hash to client
		5. client sends hash and clientUserID to server to authenticate
		6. host has his hostToken and clientUserID and can compare
		   the clients hash with his newly created hash

		with this method servers to not need to stay conntected to
		the operator
		
		*/
	};
}
