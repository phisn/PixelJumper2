#pragma once

#include <Operator/source/operator/ActiveUserContainer.h>
#include <Operator/source/operator/AuthenticationHandler.h>
#include <Operator/source/net/CommonRequestMessage.h>

namespace Operator::Net
{
	struct CommonRequestHandlerCallback
	{
		virtual void registerAsClassicHost(const Client::RegisterClassicHostMessage& message) = 0;
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
				if (Client::RegisterClassicHostMessage message; loadMessage(messageID, &message, pipe))
				{
					onRegisterClassicHost(message);
				}

				return true;

			case Client::CommonRequestMessageID::HostFindClassic:
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
					Game::Net::CommonMessageID::InternalError,
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

		void onRegisterClassicHost(const Client::RegisterClassicHostMessage& message)
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
					Game::Net::CommonMessageID::InternalError,
					NULL);
			}

			if (IsUserTrusted(type))
			{
				access->accessSendMessage(
					Host::CommonRequestMessageID::RegisterClassicHostAccepted,
					NULL);

				callback->registerAsClassicHost(message);
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

			const ClassicHost* host = ClassicHostContainer::FindHost();

			if (host == NULL)
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
				host->getConfig().host.userID);

			switch (result)
			{
			case Database::ConditionResult::Error:
				Log::Error(L"GetPlayerToken failed in findclassic host",
					host->getConfig().host.userID, L"hostID",
					userID, L"userID");

				access->accessSendMessage(
					Game::Net::CommonMessageID::InternalError,
					NULL);

				break;
			case Database::ConditionResult::NotFound:
				Log::Error(L"Failed to retrive host token in find classic host",
					host->getConfig().host.userID, L"hostID",
					userID, L"userID");

				access->accessSendMessage(
					Game::Net::CommonMessageID::InternalError,
					NULL);

				break;
			}

			keySource.userID = userID;

			Host::HostFindClassicMessage message;

			message.userID = host->getConfig().host.userID;
			message.address = host->getConfig().host.address;
			message.key.make(keySource);

			access->accessSendMessage(
				Host::CommonRequestMessageID::HostFindClassic,
				&message);
		}
	};
}
