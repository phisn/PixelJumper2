#pragma once

#include <Operator/source/operator/ActiveUserContainer.h>
#include <Operator/source/operator/AuthenticationHandler.h>
#include <Operator/source/net/CommonRequestMessage.h>

namespace Operator::Net
{
	struct CommonRequestHandlerCallback
	{
		virtual void registerAsClassicHost() = 0;
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
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Client::CommonRequestMessageID::RequestConnectionKey:
				if (Client::RequestConnectionKeyMessage message; loadMessage(messageID, &message, pipe))
				{
					onRequestConnnectionKey(message);
				}

				return true;

			case Client::CommonRequestMessageID::RegisterClassicHost:
				onRegisterClassicHost();

				return true;

			case Client::CommonRequestMessageID::HostFindClassic:
				

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
		void onRequestConnnectionKey(const Client::RequestConnectionKeyMessage& request)
		{
			ConnectionKeySource keySource;

			Database::ConditionResult result = Database::Interface::GetPlayerToken(
				keySource.token,
				request.userID);

			switch (result)
			{
			case Database::ConditionResult::NotFound:
				access->accessSendMessage(
					Host::CommonRequestMessageID::ConnectionKeyFailed,
					NULL);

				return;
			case Database::ConditionResult::Error:
				access->accessSendMessage(
					Host::CommonRequestMessageID::InternalError,
					NULL);

				return;
			}

			keySource.userID = request.userID;

			Host::ConnectionKeyMessage message;
			message.key.make(keySource);
			
			access->accessSendMessage(
				Host::CommonRequestMessageID::ConnectionKey,
				&message);
		}

		void onRegisterClassicHost()
		{
			UserType type;
			const Database::ConditionResult result = Database::Interface::GetUserType(
				type,
				userID);
			
			if (result != Database::ConditionResult::Found)
			{
				Log::Error(L"Failed to get user type",
					userID, L"userID",
					(int) result, L"result");

				access->accessSendMessage(
					Host::CommonRequestMessageID::InternalError,
					NULL);
			}

			if (IsUserTrusted(type))
			{
				access->accessSendMessage(
					Host::CommonRequestMessageID::RegisterClassicHostAccepted,
					NULL);

				callback->registerAsClassicHost();
			}
			else
			{
				access->accessSendMessage(
					Host::CommonRequestMessageID::RegisterClassicHostRejected,
					NULL);
			}
		}

		void onHostFindClassic()
		{
			if (UserContainer::GetUserMode(userID) != ActiveUserMode::Waiting)
			{
				Host::HostFindClassicRejectedMessage message;
				message.type = Host::HostFindClassicRejectedMessageContent::InvalidUserMode;

				access->accessSendMessage(
					Host::CommonRequestMessageID::HostFindClassicRejected,
					&message);

				return;
			}

			const UserID hostID = ClassicHostContainer::FindHost();

			if (hostID == NULL)
			{
				Host::HostFindClassicRejectedMessage message;
				message.type = Host::HostFindClassicRejectedMessageContent::NoHostAvailable;

				access->accessSendMessage(
					Host::CommonRequestMessageID::HostFindClassicRejected,
					&message);

				return;
			}

			ConnectionKeySource keySource;

			const Database::ConditionResult result = Database::Interface::GetPlayerToken(
				keySource.token,
				hostID);

			switch (result)
			{
			case Database::ConditionResult::Error:
				Log::Error(L"GetPlayerToken failed in findclassic host",
					hostID, L"hostID",
					userID, L"userID");

				access->accessSendMessage(
					Host::CommonRequestMessageID::InternalError,
					NULL);

				break;
			case Database::ConditionResult::NotFound:
				Log::Error(L"Failed to retrive host token in find classic host",
					hostID, L"hostID",
					userID, L"userID");

				access->accessSendMessage(
					Host::CommonRequestMessageID::InternalError,
					NULL);

				break;
			}

			keySource.userID = hostID;

			Host::HostFindClassicMessage message;

			message.userID = hostID;
			message.address;
			message.key.make(keySource);

			access->accessSendMessage(
				Host::CommonRequestMessageID::RegisterClassicHostAccepted,
				&message);
		}
	};
}
