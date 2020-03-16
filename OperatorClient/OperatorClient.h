#pragma once

#include "request/RequestInterface.h"

#include "Common/Common.h"
#include "Common/RandomModule.h"
#include "NetCore/Client.h"
#include "NetCore/message/OperatorAuthenticationMessage.h"
#include "NetCore/message/OperatorCloseReason.h"
#include "NetCore/message/OperatorRequestMessage.h"

#include <unordered_map>

namespace Operator
{
	struct ClientRequest
	{
		virtual void onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) = 0;
	};

	class Client
		:
		public ::Net::Client
	{
		// enable call to onAuthenticated
		friend class ClientAuthenticationRequest;

		struct RequestWrapper
		{
			~RequestWrapper()
			{
				if (message.payload)
					delete message.payload;
			}

			sf::Time age;

			::Net::OperatorRequestMessage message;
			RequestInterface* request;
		};

		Client(const SteamNetworkingIPAddr address)
			:
			address(address)
		{
		}

		static sf::Time processInterval;
		static sf::Time processCounter;

		static Client* client;

	public:
		enum Status
		{
			Connecting,
			Connected,
			Unconnected
		};

		enum AuthenticationStatus
		{
			Unauthenticated,
			Authenticated,
			Authenticating
		};

		static void Initialize(
			const SteamNetworkingIPAddr address,
			const sf::Time interval = sf::milliseconds(100))
		{
			processInterval = interval;

			if (client)
				delete client;

			client = new Client(address);
		}

		static void Process()
		{
		}

		enum class PushRequestFailure
		{
			Success,
			Unauthenticated, 
			Authenticating,
			ConnectingFailed,
			SendingFailed
		};

		static PushRequestFailure PushRequest(
			const ::Net::MessageID messageID,
			::Net::NetworkMessage* const message,
			RequestInterface* const request)
		{
			if (client->authenticationStatus == Authenticating)
				return PushRequestFailure::Authenticating;

			if (messageID > ::Net::Client::OperatorAuthenticationMessageID::_Begin &&
				messageID < ::Net::Client::OperatorAuthenticationMessageID::_Offset)
			{
				if (client->status == Authenticated)
					client->disconnect();

				client->authenticationStatus = Authenticating;
			}
			else
			{
				if (client->authenticationStatus == Unauthenticated)
					return PushRequestFailure::Unauthenticated;
			}

			RequestWrapper& requestWrapper = client->requests.emplace_back();

			requestWrapper.request = request;
			requestWrapper.message.content.messageID = messageID;
			requestWrapper.message.content.requestID = Module::Random::MakeRandom<::Net::RequestID>();
			requestWrapper.message.payload = message;

			switch (client->status)
			{
			case Status::Unconnected:
				if (!client->connect(client->address))
				{
					if (client->authenticationStatus == Authenticating)
						client->authenticationStatus = Unauthenticated;

					client->removeAllRequests();
					return PushRequestFailure::ConnectingFailed;
				}

			case Status::Connected:
				if (!client->sendMessage(
						::Net::Host::OperatorRequestMessageID::OperatorRequest,
						&requestWrapper.message))
				{
					if (client->authenticationStatus == Authenticating)
						client->authenticationStatus = Unauthenticated;

					// requests can already potentially be removed
					// if sendmessage failure reason was a sendfailure
					if (client->requests.size() > 0)
					{
						request->onRequestFailure(RequestInterface::Reason::Internal);
						client->requests.pop_back();
					}

					return PushRequestFailure::SendingFailed;
				}

				break;
			}

			return PushRequestFailure::Success;
		}

		static void PushRequest(
			const ::Net::MessageID messageID,
			::Net::Client::TokenMessage* const message,
			RequestInterface* const request)
		{
			memcpy(client->token.token,
				message->token,
				OPERATOR_HASH_SIZE);
			PushRequest(
				messageID,
				(::Net::NetworkMessage*) message,
				request);
		}

		static bool IsAuthenticated()
		{
			return client->status == Authenticated;
		}

		static Status GetStatus()
		{
			return client->status;
		}

		static const Operator::AuthenticationToken& GetToken()
		{
			return client->token;
		}

		static Operator::UserID GetUserID()
		{
			return client->userID;
		}

		bool disconnect(
			const int reason = 0,
			const char* const message = "disconnect",
			bool linger = false) override
		{
			status = Unconnected;
			return disconnect(reason, message, linger);
		}

	private:
		const SteamNetworkingIPAddr address;
		
		UserID userID;
		Operator::AuthenticationToken token;
		
		AuthenticationStatus authenticationStatus;
		Status status;

		std::vector<RequestWrapper> requests;

		void onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case ::Net::CommonMessageID::InternalError:


				break;
			case ::Net::CommonMessageID::ExternalError:
				
				
				break;
			case ::Net::Host::OperatorRequestMessageID::OperatorRequest:
				if (::Net::OperatorRequestMessage message; loadMessage(messageID, &message, pipe))
				{
					decltype(requests)::iterator request = std::find_if(
						requests.begin(),
						requests.end(),
						[&message](const RequestWrapper& request)
						{
							return request.message.content.requestID == message.content.requestID;
						});

					if (request == requests.end())
					{
						Log::Warning(L"Got invalid requestID from operator. a request probably already timed out",
							message.content.messageID, L"messageID",
							message.content.requestID, L"requestID");
					}
					else
					{
						if (request->request->onMessage(
								message.content.messageID,
								pipe))
						{
							requests.erase(request);
						}
					}
				}

				break;
			default:
				break;
			}
		}

		bool loadMessage(
			const ::Net::MessageID messageID,
			::Net::NetworkMessage* const message,
			Resource::ReadPipe* const pipe)
		{
			if (!message->load(pipe))
			{
				onThreatIdentified(
					messageID,
					L"failed to load message",
					::Net::ThreatLevel::Suspicious);
				accessOnRequestFailed(
					messageID,
					::Net::RequestFailure::Loading);

				return false;
			}

			return true;
		}

		// called from friend [request] ClientAuthenticationRequest
		void onAuthenticated(
			const char token[OPERATOR_HASH_SIZE],
			const UserID userID)
		{
			authenticationStatus = Authenticated;

			// null if authentication by token
			// then already stored in token
			if (token)
				memcpy(this->token.token,
					token,
					OPERATOR_HASH_SIZE);
			this->userID = userID;
		}

		void onConnectionOpened() override
		{
			Client::onConnectionOpened();
		}

		void accessOnRequestFailed(
			const ::Net::MessageID messageID, 
			const ::Net::RequestFailure reason) override
		{
			// ignore?
		}

		void onThreatIdentified(
			const sf::Uint32 identifier, 
			const std::wstring& note,
			const ::Net::ThreatLevel level) override
		{
			Log::Warning(L"Threat identified",
				identifier, L"id",
				note, L"note",
				(int)level, L"level");
		}

		void onMessageSendFailed(
			const ::Net::MessageID messageID,
			const ::Net::SendFailure failure) override
		{
			Log::Error(L"Failed to send message",
				messageID, L"messageID");

			// send failure is fatal failure
			if (failure == ::Net::SendFailure::Send)
			{
				removeAllRequests();
				disconnect();
				adjustStatus(Unconnected);
			}
		}

		void onConnectionLost(const int reason) override
		{
			Log::Error(L"connection lost from operator",
				reason, L"reason");

			removeAllRequests();
			adjustStatus(Unconnected);
		}

		void onConnectionClosed(const int reason) override
		{
			adjustStatus(Client::Status::Unconnected);

			if (requests.size() > 0)
				switch (reason)
				{
				case ::Net::OperatorCloseReason::ConnectionClosed:
					// closed connection probably caused by one of
					// the requests. reconnect is pointless
				
					removeAllRequests();

					break;
				case ::Net::OperatorCloseReason::IdleConnection:
					if (authenticationStatus != Authenticated)
					{
						Log::Information(
							L"got probably impossible authentication"
							L"status in connection closed by idle with"
							L"requests remaining",
							requests.size(), L"requests",
							(int) authenticationStatus, L"status");
					}
					
					Log::Information(L"reconnecting because of missing request in idle");
					
					if (!connect(address))
					{
						removeAllRequests();
					}

					break;
				}

		}

		void removeAllRequests()
		{

		}

		void adjustStatus(const Status status)
		{
			if (status == Unconnected)
				if (authenticationStatus == Authenticating)
				{
					authenticationStatus = Unauthenticated;
				}

			this->status = status;
		}
	};
}
