#pragma once

#include "request/RequestInterface.h"

#include "Common/Common.h"
#include "Common/RandomModule.h"
#include "NetCore/Client.h"
#include "NetCore/message/OperatorAuthenticationMessage.h"
#include "NetCore/message/OperatorCloseReason.h"
#include "NetCore/message/OperatorRequestMessage.h"

#include <cassert>
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
		public ::Net::ClientBase
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

			sf::Time timeout = sf::milliseconds(10000);
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

		static void Uninitialize()
		{
			if (client)
				delete client;
		}

		static void Process(const sf::Time time)
		{
			processCounter += time;

			if (processCounter > processInterval)
			{
				if (client->status == Connected)
					assert(client->processMessages());

				decltype(requests)::iterator request = client->requests.begin();
				while (request != client->requests.end())
				{
					request->age += processInterval;

					if (request->age > request->timeout)
					{
						// 1. unconnected
						// 2. unauthenticated
						// 3. unresponened
						request->request->onRequestFailure(
							client->status == Connected
							? (client->authenticationStatus == Authenticated
								? RequestInterface::Reason::TimeoutUnresponeded
								: RequestInterface::Reason::TimeoutUnauthenticated)
							: RequestInterface::Reason::TimeoutUnconnected);

						request = client->requests.erase(request);
					}
					else ++request;
				}

				processCounter = sf::Time::Zero;
			}
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
			if (!client->tokenKnown && client->authenticationStatus == Authenticating)
				return PushRequestFailure::Authenticating;

			if (messageID > ::Net::Client::OperatorAuthenticationMessageID::_Begin &&
				messageID < ::Net::Client::OperatorAuthenticationMessageID::_Offset)
			{
				// removing old authentication if already
				// authenticated
				if (client->status == Authenticated)
					client->disconnect();

				client->authenticationStatus = Authenticating;
				client->tokenKnown = false;
			}
			else
			{
				if (!client->tokenKnown)
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
					// connect is fatal failure. if currently authenticating
					// cancel
					client->authenticationStatus = Unauthenticated;
					client->removeAllRequests(RequestInterface::Reason::ConnectFailed);

					return PushRequestFailure::ConnectingFailed;
				}

				break;
			case Status::Connected:
				if (client->tokenKnown && client->authenticationStatus == Authenticating)
				{
					// operator currently reauthenticating
					// sending this message later after authenticating

					break;
				}

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
						request->onRequestFailure(RequestInterface::Reason::SendFailed);
						client->requests.pop_back();
					}

					return PushRequestFailure::SendingFailed;
				}

				break;
			}

			return PushRequestFailure::Success;
		}

		static PushRequestFailure PushRequest(
			const ::Net::MessageID messageID,
			::Net::Client::TokenMessage* const message,
			RequestInterface* const request)
		{
			memcpy(client->token.token,
				message->token,
				OPERATOR_HASH_SIZE);
			return PushRequest(
				messageID,
				(::Net::NetworkMessage*) message,
				request);
		}

		static bool AuthenticationTokenKnown()
		{
			return client->tokenKnown;
		}

		static AuthenticationStatus GetAuthenticationStatus()
		{
			return client->authenticationStatus;
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
			authenticationStatus = Unauthenticated;
			status = Unconnected;

			if (requests.size() > 0)
			{
				removeAllRequests(RequestInterface::Reason::Disconnect);
			}

			return ClientBase::disconnect(reason, message, linger);
		}

	private:
		const SteamNetworkingIPAddr address;
		
		UserID userID = NULL;
		Operator::AuthenticationToken token;
		bool tokenKnown = false;

		// represents authentication state in
		// the view of operator. to see if already
		// authenticated check tokenKnown
		AuthenticationStatus authenticationStatus = Unauthenticated;
		Status status = Unconnected;

		std::vector<RequestWrapper> requests;

		void onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case ::Net::CommonMessageID::InternalError:
				// operator is normally going to close connection soon
				// until then ignore this fact and hope it wont but still
				// remove all requests because one of them can be bad

				if (authenticationStatus == Authenticating)
				{
					Log::Error(L"reauthentication failed with internal error",
						(int) status, L"status",
						tokenKnown, L"tokenKnown",
						userID, L"userID");

					removeAllRequests(RequestInterface::Reason::AuthenticateFailed);
				}
				else
				{
					Log::Error(L"got internal error from operator",
						(int) status, L"status",
						(int) authenticationStatus, L"authenticationstatus",
						tokenKnown, L"tokenKnown",
						userID, L"userID");

					removeAllRequests(RequestInterface::Reason::InternalError);
				}

				break;
			case ::Net::CommonMessageID::ExternalError:
				if (::Net::ExternalErrorMessage message; loadMessage(messageID, &message, pipe))
				{
					if (authenticationStatus == Authenticating)
					{
						Log::Error(L"reauthentication failed with external error",
							(int)status, L"status",
							tokenKnown, L"tokenKnown",
							userID, L"userID",
							message.message, L"message",
							message.content.messageID, L"messageID",
							message.content.errorID, L"errorID");

						removeAllRequests(RequestInterface::Reason::AuthenticateFailed);
					}
					else
					{
						Log::Error(L"got external error from operator",
							(int)status, L"status",
							(int)authenticationStatus, L"authenticationstatus",
							tokenKnown, L"tokenKnown",
							userID, L"userID",
							message.message, L"message",
							message.content.messageID, L"messageID",
							message.content.errorID, L"errorID");

						removeAllRequests(RequestInterface::Reason::InternalError);
					}
				}
				else
				{
					if (authenticationStatus == Authenticating)
					{
						Log::Error(L"reauthentication failed with external error (message unloadable)",
							(int)status, L"status",
							tokenKnown, L"tokenKnown",
							userID, L"userID");

						removeAllRequests(RequestInterface::Reason::AuthenticateFailed);
					}
					else
					{
						Log::Error(L"got external error from operator (message unloadable)",
							(int)status, L"status",
							(int)authenticationStatus, L"authenticationstatus",
							tokenKnown, L"tokenKnown",
							userID, L"userID");

						removeAllRequests(RequestInterface::Reason::ExternalError);
					}
				}
				
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
			case ::Net::Host::OperatorAuthenticationMessageID::AcceptToken:
				if (::Net::Host::AcceptTokenMessage message; loadMessage(messageID, &message, pipe))
				{
					if (userID != message.userID)
					{
						Log::Error(L"got invalid userid in reauthentication",
							userID, L"expected",
							message.userID, L"received");

						authenticationStatus = Unauthenticated;
						removeAllRequests(RequestInterface::Reason::AuthenticateFailed);

						break;
					}

					authenticationStatus = Authenticated;
					resendAllRequests();
				}

				break;
			case ::Net::Host::OperatorAuthenticationMessageID::RejectToken:
				Log::Error(L"token for reauthentication rejected. probably expired");

				removeAllRequests(RequestInterface::Reason::AuthenticateFailed);
				authenticationStatus = Unauthenticated;

				break;
			default:
				Log::Error(L"got unkown messageid from operator",
					messageID, L"messageID");

				assert(false);

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
			tokenKnown = true;

			// null if authentication by token
			// then already stored in token
			if (token)
				memcpy(this->token.token,
					token,
					OPERATOR_HASH_SIZE);
			this->userID = userID;

			// will not be called on restore
			// request count can not be not one
			assert(requests.size() == 1);
		}

		void onAuthenticationFailed()
		{
			authenticationStatus = Unauthenticated;

			// is only a single request (authenticationrequest) 
			// which does already know of this
		}

		void onConnectionOpened() override
		{
			::Net::ClientBase::onConnectionOpened();
			status = Connected;

			if (tokenKnown)
			{
				::Net::Client::TokenMessage message;

				memcpy(message.token,
					token.token,
					OPERATOR_HASH_SIZE);

				if (sendMessage(
						::Net::Client::OperatorAuthenticationMessageID::Token,
						&message))
				{
					authenticationStatus = Authenticating;
				}
				else
				{
					tokenKnown = false;
					authenticationStatus = Unauthenticated;

					removeAllRequests(RequestInterface::Reason::AuthenticateFailed);
				}

				//////////////////////////////////
				// authentication restore missing
			}
			else
			{
				resendAllRequests();
			}
		}

		void accessOnRequestFailed(
			const ::Net::MessageID messageID, 
			const ::Net::RequestFailure reason) override
		{
			Log::Warning(L"request failed",
				messageID, L"messageID",
				(int) reason, L"reason");
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
				// already removes all requests
				disconnect();
			}
		}

		void onConnectionLost(const int reason) override
		{
			Log::Warning(L"operator connection lost",
				reason, L"reason");

			removeAllRequests(RequestInterface::Reason::ConnectionLost);

			status = Unconnected;
			authenticationStatus = Unauthenticated;
		}

		void onConnectionClosed(const int reason) override
		{
			Log::Information(L"operator connection closed",
				reason, L"reason");

			status = Unconnected;
			authenticationStatus = Unauthenticated;

			if (requests.size() > 0)
				switch (reason)
				{
				case ::Net::OperatorCloseReason::ConnectionClosed:
					// closed connection probably caused by one of
					// the requests. reconnect is pointless
				
					removeAllRequests(RequestInterface::Reason::ConnectionClosed);

					break;
				case ::Net::OperatorCloseReason::IdleConnection:
					if (!tokenKnown)
					{
						Log::Information(
							L"got probably impossible authentication"
							L"status in connection closed by idle with"
							L"requests remaining",
							requests.size(), L"requests",
							(int) authenticationStatus, L"status");

						break;
					}
					
					Log::Information(L"reconnecting because of missing request in idle");
					
					if (!connect(address))
					{
						removeAllRequests(RequestInterface::Reason::ConnectFailed);
					}

					break;
				}

		}

		void resendAllRequests()
		{
			decltype(requests)::iterator request = requests.begin();
			
			while (request != requests.end())
				if (sendMessage(
						::Net::Host::OperatorRequestMessageID::OperatorRequest,
						&request->message))
				{
					++request;
				}
				else
				{
					request = requests.erase(request);
				}
		}

		void removeAllRequests(const RequestInterface::Reason reason)
		{
			for (RequestWrapper& request : requests)
			{
				request.request->onRequestFailure(reason);
			}

			requests.clear();
		}
	};
}
