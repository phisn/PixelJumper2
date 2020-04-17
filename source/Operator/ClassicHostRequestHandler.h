#pragma once

#include "ActiveUserContainer.h"
#include "ActiveHostContainer.h"
#include "database/DatabaseInterface.h"
#include "database/UnlockedRepresentationsTable.h"
#include "database/UnlockedWorldsTable.h"

#include "Common/Common.h"
#include "NetCore/message/OperatorClassicSimulatorMessage.h"
#include "NetCore/RequestHandler.h"

namespace Operator
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
			const ::Net::Client::RegisterClassicHostMessage& message,
			SteamNetworkingIPAddr address)
			:
			userID(userID),
			hostContainer(userID)
		{
			hostContainer.maxPlayers = message.content.maxPlayers;
			hostContainer.host.address = address;
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
			case Net::Client::OperatorClassicHostID::RegisterClient:
				if (Net::Client::OperatorClassicHost::RegisterClientMessage message; loadMessage(messageID, &message, pipe))
				{
					onRegisterClient(message);
				}

				return true;
			case Net::Client::OperatorClassicHostID::UnregisterClient:
				if (Net::Client::OperatorClassicHost::UnregisterClientMessage message; loadMessage(messageID, &message, pipe))
				{
					onUnregisterClient(message);
				}

				return true;
			case Net::Client::OperatorClassicHostID::ClientData:
				if (Net::Client::OperatorClassicHost::RequestClientDataMessage message; loadMessage(messageID, &message, pipe))
				{
					onRequestClientData(message);
				}

				return true;

			case Net::Client::OperatorClassicHostID::UnlockWorld:
				if (Net::Client::OperatorClassicHost::UnlockWorldMessage message; loadMessage(messageID, &message, pipe))
				{
					onUnlockWorld(message);
				}

				return true;

			case Net::Client::OperatorClassicHostID::UnlockRepresentation:
				if (Net::Client::OperatorClassicHost::UnlockRepresentationMessage message; loadMessage(messageID, &message, pipe))
				{
					onUnlockRepresentation(message);
				}

				return true;
			}

			return false;
		}

	private:
		const UserID userID;
		ClassicHostContainer hostContainer;

		void onRegisterClient(const ::Net::Client::OperatorClassicHost::RegisterClientMessage& request)
		{
			switch (hostContainer.registerUser(request.content.userID))
			{
			case ClassicHostContainer::RegisterUserFailure::MaxUserCount:
			{
				sendRegisterClientFailure(
					::Net::Host::OperatorClassicHost::ClientRegistrationFailedReason::MaxPlayerCountReached);

				return;
			}
			case ClassicHostContainer::RegisterUserFailure::UserAlreadyHosted:
			{
				sendRegisterClientFailure(
					::Net::Host::OperatorClassicHost::ClientRegistrationFailedReason::UserRegisteredSomewhere);

				return;
			}
			}

			::Net::Host::OperatorClassicHost::ClientRegisteredMessage message;

			// this might be doubled code but is in any way better
			// than some hacks to make this unneccessary shorter than
			// it has to be. since there wont be anything added, this
			// is fine
			Database::ConditionResult result = RetrivePlayerResource(
				&message.resource,
				userID);

			if (result != Database::ConditionResult::Found)
			{
				switch (result)
				{
				case Database::ConditionResult::Error:
					Log::Error(L"failed to retrive playerresource in registerclient, internal error",
						userID, L"userID",
						this->userID, L"our");

					sendRegisterClientFailure(
						::Net::Host::OperatorClassicHost::ClientRegistrationFailedReason::DatabaseRetriveFailed);

					break;
				case Database::ConditionResult::NotFound:
					Log::Error(L"failed to retrive playerresource in registerclient, player not found",
						userID, L"userID",
						this->userID, L"our");

					sendRegisterClientFailure(
						::Net::Host::OperatorClassicHost::ClientRegistrationFailedReason::UserNotFound);

					break;
				}

				hostContainer.unregisterUser(userID);
			}

			if (!RetriveClassicPlayerResource(
					&message.classicResource,
					userID))
			{
				Log::Error(L"failed to retrive classicplayerresource in registerclient, internal error",
					userID, L"userID",
					this->userID, L"our");

				sendRegisterClientFailure(
					::Net::Host::OperatorClassicHost::ClientRegistrationFailedReason::DatabaseRetriveFailed);
				return;
			}

			if (!access->sendMessage(
					::Net::Host::OperatorClassicHostID::ClientRegistered,
					&message))
			{
				hostContainer.unregisterUser(userID);
			}
		}

		void sendRegisterClientFailure(::Net::Host::OperatorClassicHost::ClientRegistrationFailedReason reason)
		{
			::Net::Host::OperatorClassicHost::ClientRegistrationFailedMessage message;
			message.content.reason = reason;

			access->sendMessage(
				::Net::Host::OperatorClassicHostID::ClientRegistrationFailed,
				&message);
		}

		void onUnregisterClient(const ::Net::Client::OperatorClassicHost::UnregisterClientMessage& message)
		{
			hostContainer.unregisterUser(message.content.userID);
			access->sendMessage(::Net::Host::OperatorClassicHostID::ClientUnregistered);
		}

		void onRequestClientData(::Net::Client::OperatorClassicHost::RequestClientDataMessage& request)
		{
			::Net::Host::OperatorClassicHost::ClientDataMessage message;
			Database::ConditionResult result = RetrivePlayerResource(
				&message.resource,
				userID);

			if (result != Database::ConditionResult::Found)
			{
				switch (result)
				{
				case Database::ConditionResult::Error:
					Log::Error(L"failed to retrive playerresource in requestclientdata, internal error",
						userID, L"userID",
						this->userID, L"our");

					sendRequestClientDataFailed(
						::Net::Host::OperatorClassicHost::ClientDataRequestFailedReason::DatabaseRetriveFailed);

					break;
				case Database::ConditionResult::NotFound:
					Log::Error(L"failed to retrive playerresource in requestclientdata, player not found",
						userID, L"userID",
						this->userID, L"our");

					sendRequestClientDataFailed(
						::Net::Host::OperatorClassicHost::ClientDataRequestFailedReason::InvalidUserID);

					break;
				}
			}

			if (!RetriveClassicPlayerResource(
					&message.classicResource,
					userID))
			{
				Log::Error(L"failed to retrive classicplayerresource in requestclientdata, internal error",
					userID, L"userID",
					this->userID, L"our");

				sendRequestClientDataFailed(
					::Net::Host::OperatorClassicHost::ClientDataRequestFailedReason::DatabaseRetriveFailed);

				return;
			}

			access->sendMessage(
				::Net::Host::OperatorClassicHostID::ClientDataReply,
				&message);
		}

		void onUnlockWorld(const ::Net::Client::OperatorClassicHost::UnlockWorldMessage& message)
		{
			if (UnlockWorld(message.content.userID, message.content.worldID))
			{
				access->sendMessage(
					::Net::Host::OperatorClassicHostID::UnlockedWorld,
					NULL);
			}
			else
			{
				Log::Error(L"failed to unlock world",
					message.content.userID, L"userID",
					hostContainer.host.userID, L"hostID",
					message.content.worldID, L"worldID");

				access->sendMessage(
					::Net::Host::OperatorClassicHostID::UnlockWorldFailed,
					NULL);
			}
		}

		void onUnlockRepresentation(const Net::Client::OperatorClassicHost::UnlockRepresentationMessage& message)
		{
			if (UnlockRepresentation(message.content.userID, message.content.representationID))
			{
				access->sendMessage(
					::Net::Host::OperatorClassicHostID::UnlockedRepresentation,
					NULL);
			}
			else
			{
				Log::Error(L"failed to unlock representation",
					message.content.userID, L"userID",
					hostContainer.host.userID, L"hostID",
					message.content.representationID, L"representationID");

				access->sendMessage(
					::Net::Host::OperatorClassicHostID::UnlockedRepresentation,
					NULL);
			}
		}

		void sendRequestClientDataFailed(::Net::Host::OperatorClassicHost::ClientDataRequestFailedReason reason)
		{
			::Net::Host::OperatorClassicHost::ClientDataRequestFailedMessage message;
			message.content.reason = reason;

			access->sendMessage(
				::Net::Host::OperatorClassicHostID::ClientDataRequestFailed,
				&message);
		}
	};
}
