#pragma once

#include "ActiveUserContainer.h"
#include "AuthenticationHandler.h"

#include "NetCore/message/OperatorCommonMessage.h"

namespace Operator::Net
{
	struct CommonRequestHandlerCallback
	{
		virtual void registerAsClassicHost(const ::Net::Client::RegisterClassicHostMessage& message) = 0;
	};

	class CommonRequestHandler
		:
		public ::Net::RequestHandler
	{
	public:
		CommonRequestHandler(
			CommonRequestHandlerCallback* const callback,
			const UserID userID)
			:
			callback(callback),
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
			case ::Net::Client::OperatorCommonMessageID::RequestConnectionKey:
				if (::Net::Client::RequestConnectionKeyMessage message; loadMessage(messageID, &message, pipe))
				{
					onRequestConnnectionKey(message);
				}

				return true;

			case ::Net::Client::OperatorCommonMessageID::RegisterClassicHost:
				if (::Net::Client::RegisterClassicHostMessage message; loadMessage(messageID, &message, pipe))
				{
					onRegisterClassicHost(message);
				}

				return true;

			case ::Net::Client::OperatorCommonMessageID::HostFindClassic:
				onHostFindClassic();

				return true;
			}

			return false;
		}

	private:
		CommonRequestHandlerCallback* const callback;
		const UserID userID;

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
		void onRequestConnnectionKey(const ::Net::Client::RequestConnectionKeyMessage& request)
		{
			ConnectionKeySource keySource;

			Database::ConditionResult result = DatabaseInterface::GetPlayerToken(
				keySource.token,
				request.userID);

			switch (result)
			{
			case Database::ConditionResult::NotFound:
				access->sendMessage(
					::Net::Host::OperatorCommonMessageID::ConnectionKeyFailed,
					NULL);

				return;
			case Database::ConditionResult::Error:
				access->sendMessage(
					::Net::CommonMessageID::InternalError,
					NULL);

				return;
			}

			keySource.userID = request.userID;

			::Net::Host::ConnectionKeyMessage message;
			message.key.make(keySource);
			
			access->sendMessage(
				::Net::Host::OperatorCommonMessageID::ConnectionKey,
				&message);
		}

		void onRegisterClassicHost(const ::Net::Client::RegisterClassicHostMessage& message)
		{
			UserType type;
			const Database::ConditionResult result = DatabaseInterface::GetUserType(
				type,
				userID);
			
			if (result != Database::ConditionResult::Found)
			{
				Log::Error(L"Failed to get user type",
					userID, L"userID",
					(int) result, L"result");

				access->sendMessage(
					::Net::CommonMessageID::InternalError,
					NULL);
			}

			if (IsUserTrusted(type))
			{
				access->sendMessage(
					::Net::Host::OperatorCommonMessageID::RegisterClassicHostAccepted,
					NULL);

				callback->registerAsClassicHost(message);
			}
			else
			{
				access->sendMessage(
					::Net::Host::OperatorCommonMessageID::RegisterClassicHostRejected,
					NULL);
			}
		}

		void onHostFindClassic()
		{
			if (UserContainer::GetUserMode(userID) != ActiveUserMode::Waiting)
			{
				::Net::Host::HostFindClassicRejectedMessage message;
				message.type = ::Net::Host::HostFindClassicRejectedMessageContent::InvalidUserMode;

				access->sendMessage(
					::Net::Host::OperatorCommonMessageID::HostFindClassicRejected,
					&message);

				return;
			}

			const ClassicHost* host = ClassicHostContainer::FindHost();

			if (host == NULL)
			{
				::Net::Host::HostFindClassicRejectedMessage message;
				message.type = ::Net::Host::HostFindClassicRejectedMessageContent::NoHostAvailable;

				access->sendMessage(
					::Net::Host::OperatorCommonMessageID::HostFindClassicRejected,
					&message);

				return;
			}

			ConnectionKeySource keySource;

			const Database::ConditionResult result = DatabaseInterface::GetPlayerToken(
				keySource.token,
				host->getConfig().host.userID);

			switch (result)
			{
			case Database::ConditionResult::Error:
				Log::Error(L"GetPlayerToken failed in findclassic host",
					host->getConfig().host.userID, L"hostID",
					userID, L"userID");

				access->sendMessage(
					::Net::CommonMessageID::InternalError,
					NULL);

				break;
			case Database::ConditionResult::NotFound:
				Log::Error(L"Failed to retrive host token in find classic host",
					host->getConfig().host.userID, L"hostID",
					userID, L"userID");

				access->sendMessage(
					::Net::CommonMessageID::InternalError,
					NULL);

				break;
			}

			keySource.userID = userID;

			::Net::Host::HostFindClassicMessage message;

			message.userID = host->getConfig().host.userID;
			message.address = host->getConfig().host.address;
			message.key.make(keySource);

			access->sendMessage(
				::Net::Host::OperatorCommonMessageID::HostFindClassic,
				&message);
		}
	};
}
