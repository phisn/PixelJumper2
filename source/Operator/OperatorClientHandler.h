#pragma once

#include "AuthenticationHandler.h"
#include "ClassicHostRequestHandler.h"
#include "CommonRequestHandler.h"

#include "NetCore/ClientHandler.h"

namespace Operator::Net
{
	typedef sf::Uint32 Age;

	class OperatorClientHandler
		:
		public ::Net::ClientHandler,
		public ::Net::RequestContainer,
		public AuthenticationHandlerCallback,
		public CommonRequestHandlerCallback
	{
		ConnectionAccess* getConnectionAccess() override
		{
			return this;
		}

	public:
		enum class Status
		{
			Authenticating,
			Authenticated,

			// user is officially hosting and
			// should not be disconnected
			Hosting,

			// cause for closing is not specified
			// it can be an error, timeout, rejection
			// or different. if bad that it is already
			// reported as threat
			Closing
		};

		OperatorClientHandler(
			const HSteamNetConnection connection,
			const sf::Uint32 timeout = 100)
			:
			ClientHandler(connection)
		{
			addRequestHandler(
				new AuthenticationHandler(
					this,
					timeout)
			);
		}

		virtual void update()
		{
			if (!processMessages())
			{
				status = Status::Closing;

				// think about removing this because
				// it has a very high chance to fail
				// (gurranted?)
				sendMessage(::Net::CommonMessageID::InternalError);

				return;
			}

			++age;

			callHandlersUpdate();
		}

		Status getStatus() const
		{
			return status;
		}

		bool isAuthenticated() const
		{
			return status == Status::Authenticated;
		}

		Age getAge() const
		{
			return age;
		}

	private:
		using ClientHandler::processMessages;

		UserID userID = NULL;
		Status status = Status::Authenticating;
		Age age = 0;

		// when client send message with request header
		// active request mode for all sent messages from
		// this clienthandler
		bool requestModeActive = false;
		::Net::RequestID requestID;

		void onAuthenticated(const UserID userID) override
		{
			Log::Information(L"Client authenticated",
				userID, L"userID");

			status = Status::Authenticated;
			this->userID = userID;

			delete removeRequestHandler<AuthenticationHandler>();
			addRequestHandler<CommonRequestHandler>(
				new CommonRequestHandler(
					this,
					userID)
			);
		}

		void onAuthenticationDenied() override
		{
			status = Status::Closing;
		}

		void registerAsClassicHost(const ::Net::Client::RegisterClassicHostMessage& message) override
		{
			if (status != Status::Hosting)
			{
				status = Status::Hosting;

				addRequestHandler<ClassicHostRequestHandler>(
					new ClassicHostRequestHandler(
						userID)
				);

				ClassicHostConfig config;

				SteamNetConnectionInfo_t connectionInfo;
				if (!networkInterface->GetConnectionInfo(
						connection,
						&connectionInfo))
				{
					Log::Error(L"Failed to retrive connection info");
					status = Status::Closing;
				}

				config.host.address = connectionInfo.m_addrRemote;
				config.host.address.m_port = message.port;
				config.host.userID = userID;
				config.maxPlayers = message.maxPlayers;

				ClassicHostContainer::CreateHost(config);
			}
		}

		void onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			if (messageID == ::Net::Client::OperatorRequestMessageID::OperatorRequest)
			{
				::Net::OperatorRequestMessage request;

				if (!request.load(pipe))
				{
					onThreatIdentified(
						messageID,
						L"invalid operator request",
						::Net::ThreatLevel::Uncommon);
					status = Status::Closing;

					return;
				}

				requestID = request.content.requestID; 
				requestModeActive = true;

				rawOnMessage(request.content.messageID, pipe);
				requestModeActive = false;
			}
			else
			{
				rawOnMessage(messageID, pipe);
			}
		}

		void rawOnMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe)
		{
			if (!callHandlersOnMessage(messageID, pipe))
			{
				onThreatIdentified(
					messageID, 
					L"invalid messageID", 
					::Net::ThreatLevel::Suspicious);
			}
		}

		bool sendMessage(
			const ::Net::MessageID messageID,
			::Net::NetworkMessage* const message = NULL,
			const int flags = k_nSteamNetworkingSend_Reliable) override
		{
			if (requestModeActive)
			{
				::Net::OperatorRequestMessage interceptor;

				interceptor.payload = message;
				interceptor.content.messageID = messageID;
				interceptor.content.requestID = requestID;

				return ClientHandler::sendMessage(
					::Net::Host::OperatorRequestMessageID::OperatorRequest,
					&interceptor, flags);
			}

			return ClientHandler::sendMessage(
				messageID,
				message,
				flags);
		}

		void onThreatIdentified(
			const sf::Uint32 identifier,
			const std::wstring& note,
			const ::Net::ThreatLevel level) override
		{
			Log::Warning(L"Threat identified (" + note + L")",
				identifier, L"identifier",
				(int) level, L"level");
		}

		void onMessageSendFailed(
			const ::Net::MessageID messageID,
			const ::Net::SendFailure reason) override
		{
			Log::Error(L"Failed to send message");
			
			sendMessage(
				::Net::CommonMessageID::InternalError,
				NULL);
			status = Status::Closing;
		}

		void accessOnRequestFailed(
			const ::Net::MessageID messageID,
			const ::Net::RequestFailure reason) override
		{
			sendMessage(
				::Net::CommonMessageID::InternalError,
				NULL);
			status = Status::Closing;
		}
	};
}
