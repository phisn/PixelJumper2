
#pragma once

#include <Client/source/operator/request/OperatorRequest.h>

#include <Operator/source/net/AuthenticationMessage.h>
#include <Operator/source/net/ClassicSimulatorMessage.h>
#include <Operator/source/net/CommonRequestMessage.h>

#include <functional>

// game runs:
// authentication scene is pushed first
// authentication scene shows loading screen
// authentication asks operatoraccess auth
//  with stored token or credentials when
//  possible
// authentication asks cliend for credentials
//  when not stored or old ones do not work
// authentication scene pops and
//  operatoraccess status shows result
// operatoraccess accepts a eventhandler to
//  push events to like disconnected or
//  request answers?

namespace Operator
{
	enum class Status
	{
		// currently connecting to operator
		Connecting,
		// connected to operator but not authenticated
		// timeout if not authenticated in time
		Connected,
		// not connected to operator
		// caused by timeout or invalid
		// authentication
		Unconnected,
		// same as unconnected but caused
		// by an unexpected error
		Error
	};

	class ConnectionHandler
		:
		private Device::Net::Client
	{
		class RequestContainer
		{
		public:
			RequestContainer(
				Request* const request,
				const sf::Time timeout)
				:
				request(request),
				timeout(timeout)
			{
			}

			bool processForTimeout(const sf::Time time)
			{
				this->time += time;

				if (this->time > timeout)
				{
					request->onRequestFailed(Request::Reason::Timeout);
					return true;
				}

				return false;
			}

			Request* request;

		private:
			sf::Time timeout;
			sf::Time time;
		};

		struct NetworkMessageContainer
		{
			Device::Net::MessageID messageID;
			Game::Net::NetworkMessage* message;
			Request* request;
		};

		ConnectionHandler(const SteamNetworkingIPAddr ipAddress)
			:
			ipAddress(ipAddress)
		{
		}

		static ConnectionHandler* instance;

		static sf::Time processDelay;
		static sf::Time processCounter;

	public:
		// process delay for operator connection does not have
		// to be under 100ms
		static void Initialize(
			const SteamNetworkingIPAddr ipAddress,
			const sf::Time processDelay = sf::milliseconds(100))
		{
			ConnectionHandler::processDelay = processDelay;
			instance = new ConnectionHandler(ipAddress);
		}

		static void Uninitialize()
		{
			if (instance)
				delete instance;
		}

		static void Process(const sf::Time time)
		{
			processCounter += time;

			if (processCounter > processDelay)
			{
				processCounter = sf::Time::Zero;
				instance->process();
			}

			instance->processLogic(time);
		}

		static bool PushRequest(
			const Device::Net::MessageID messageID,
			Game::Net::NetworkMessage* const message,
			Request* const request)
		{
			// save message permamently because of possible lose
			// when operator disconnects in same time
			if (instance->status == Status::Connected)
			{
				if (!instance->sendCommonMessage(messageID, message))
				{
					return false;
				}

				delete message;
			}
			else
			{
				if (!instance->connect(instance->ipAddress))
				{
					return false;
				}

				instance->pendingMessages.push_back(
					NetworkMessageContainer{ messageID, message, request }
				);
			}

			instance->requests.emplace_back(request, sf::seconds(20));

			return true;
		}

		static void PopRequest(Request* const request)
		{
			return instance->removeRequest(request);
		}

		bool process() override
		{
			return Client::process();
		}

		void processLogic(const sf::Time time)
		{
			decltype(requests)::iterator iterator = requests.begin();

			while (iterator != requests.end())
			{
				if (iterator->processForTimeout(time))
				{
					requests.erase(iterator);
				}
				else
				{
					++iterator;
				}
			}
		}

		static Status GetStatus()
		{
			return instance->status;
		}

		static bool IsAuthenticated()
		{
			return instance->authenticated;
		}

		static UserID GetUserID()
		{
			return instance->userID;
		}

		static const AuthenticationToken& GetToken()
		{
			return instance->token;
		}

	private:
		const SteamNetworkingIPAddr ipAddress;

		// authenticated means that the stored token is valid
		// but does not specify weather a connection is established
		// or not. if a connection is not established then the
		// connection is idle and will be restored when a requests
		// is made. requests made while unconnected are pushed to
		// pendingmessages
		bool authenticated = false;
		Status status = Status::Unconnected;

		UserID userID;
		AuthenticationToken token;

		std::vector<RequestContainer> requests;
		std::vector<NetworkMessageContainer> pendingMessages;

		void onMessage(
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
				// common
			case Game::Net::CommonMessageID::InternalError:
				processRequests(messageID, NULL);
				status = Status::Error;
				failRequests(Request::Reason::OperatorInternalError);

				break;
				// authentication
			case Operator::Net::Host::AuthMessageID::AcceptRegistration:
			{
				Operator::Net::Host::AcceptRegistrationMessage message;

				if (!loadCommonMessage(messageID, &message, pipe))
				{
					break;
				}

				onAcceptRegistration(message);
				processRequests(messageID, (void*)&message);
				break;
			}
			case Operator::Net::Host::AuthMessageID::RejectRegistration:
			{
				Operator::Net::Host::RejectRegistrationMessage message;

				if (!message.load(pipe))
				{
					onThreatIdentified(
						messageID,
						L"invalid messagecontent",
						Device::Net::ThreatLevel::Uncommon);

					return;
				}

				onRejectRegistration(message);
				processRequests(messageID, (void*)&message);
				break;
			}
			case Operator::Net::Host::AuthMessageID::AcceptToken:
			{
				Operator::Net::Host::AcceptTokenMessage message;

				if (!message.load(pipe))
				{
					onThreatIdentified(
						messageID,
						L"invalid messagecontent",
						Device::Net::ThreatLevel::Uncommon);

					return;
				}

				onAcceptToken(message);
				processRequests(messageID, (void*)&message);
				break;
			}
			case Operator::Net::Host::AuthMessageID::RejectToken:
				processRequests(messageID, NULL);

				break;
			case Operator::Net::Host::AuthMessageID::AcceptAuthentication:
			{
				Operator::Net::Host::AcceptAuthenticationMessage message;

				if (!message.load(pipe))
				{
					onThreatIdentified(
						messageID,
						L"invalid messagecontent",
						Device::Net::ThreatLevel::Uncommon);

					return;
				}

				onAcceptAuthentication(message);
				processRequests(messageID, (void*)&message);
				break;
			}
			case Operator::Net::Host::AuthMessageID::RejectAuthentication:
				processRequests(messageID, NULL);

				break;
			case Operator::Net::Host::AuthMessageID::Timeout:
				processRequests(messageID, NULL);
				failRequests(Request::Reason::OperatorTimeout);

				break;
				// classic
			case Operator::Net::Host::ClassicHostID::RequestClientData:


				break;
			case Operator::Net::Host::ClassicHostID::RequestClientDataFailed:


			default:
				onThreatIdentified(messageID,
					L"operator invalid messageid",
					Device::Net::ThreatLevel::Suspicious);

				break;
			}
		}

		void onThreatIdentified(
			const Device::Net::MessageID messageID,
			const wchar_t* const note,
			const Device::Net::ThreatLevel level) override
		{
			Log::Warning(L"Threat identified (" + std::wstring(note) + L")",
				messageID, L"messageID",
				(int)level, L"level");
		}

		void onConnectionOpen() override
		{
			status = Status::Connected;

			// authentication requests happen before the authentication
			// a connection is only established when a attempt is made to
			// authenticate or send a request. because common requsts need
			// authentication the authentication is restored first and
			// pending messages restored later. for have to be send now
			if (authenticated)
			{
				restoreAuthentication();
			}
			else
			{
				processPendingMessages();
			}
		}

		void onConnectionLost(const int reason) override
		{
			status = Status::Unconnected;
			failRequests(Request::Reason::ConnectionLost);
		}

		void onConnectionClosed(const int reason) override
		{
			status = Status::Unconnected;
			failRequests(Request::Reason::ConnectionClosed);
		}

		void onAcceptRegistration(const Operator::Net::Host::AcceptRegistrationMessage& message)
		{
			Log::Information(L"Registration accepted");

			memcpy(
				token.token,
				message.authenticationToken,
				OPERATOR_HASH_SIZE);

			userID = message.userID;
			authenticated = true;
		}

		void onRejectRegistration(const Operator::Net::Host::RejectRegistrationMessage& message)
		{
			Log::Information(L"Registration rejected",
				(int)message.reason, L"reason");
		}

		void onAcceptAuthentication(const Operator::Net::Host::AcceptAuthenticationMessage& message)
		{
			Log::Information(L"Authentication accepted");

			memcpy(
				token.token,
				message.authenticationToken,
				OPERATOR_HASH_SIZE);

			userID = message.userID;
			authenticated = true;
		}

		void onAcceptToken(const Operator::Net::Host::AcceptTokenMessage& message)
		{
			Log::Information(L"Token accepted");

			userID = message.userID;

			// the client can already be authenticated when the connection
			// was idle and restored because of a new requsts. this requests
			// has now to be send
			if (authenticated)
			{
				processPendingMessages();
			}
			else
			{
				authenticated = true;
			}
		}

		void removeRequest(Request* const request)
		{
			for (decltype(requests)::iterator iterator = requests.begin(); iterator != requests.end(); ++iterator)
				if (iterator->request == request)
				{
					requests.erase(iterator);
					break;
				}
		}

		void failRequests(const Request::Reason reason)
		{
			for (RequestContainer& request : requests)
			{
				request.request->onRequestFailed(reason);
			}

			for (NetworkMessageContainer& message : pendingMessages)
				delete message.message;

			// where no requsts there no pending messages
			requests.clear();
			pendingMessages.clear();
		}

		void restoreAuthentication()
		{
			Operator::Net::Client::TokenMessage message;

			memcpy(message.token,
				token.token,
				OPERATOR_HASH_SIZE);

			if (!sendCommonMessage(
				Operator::Net::Client::AuthMessageID::Token,
				&message))
			{
				authenticated = false;
				failRequests(Request::Reason::AuthenticationFailed);
			}
		}

		void processPendingMessages()
		{
			for (NetworkMessageContainer& container : pendingMessages)
			{
				if (!sendCommonMessage(
					container.messageID,
					container.message))
				{
					removeRequest(container.request);
				}

				delete container.message;
			}

			pendingMessages.clear();
		}

		void processRequests(
			const Device::Net::MessageID messageID,
			void* const message)
		{
			decltype(requests)::iterator iterator = requests.begin();

			while (iterator != requests.end())
			{
				if (iterator->request->processEvent(messageID, message))
				{
					iterator = requests.erase(iterator);
				}
				else
				{
					++iterator;
				}
			}
		}

		bool sendCommonMessage(
			const Device::Net::MessageID messageID,
			Game::Net::NetworkMessage* const message)
		{
			if (!message->save(beginMessage(messageID)) || !sendMessage())
			{
				Log::Error(L"Unable to construct message",
					(sf::Uint64) messageID, L"messageID");

				return false;
			}

			return true;
		}

		bool loadCommonMessage(
			const Device::Net::MessageID messageID,
			Game::Net::NetworkMessage* const message,
			Resource::ReadPipe* const pipe)
		{
			if (!message->load(pipe))
			{
				onThreatIdentified(
					messageID,
					L"operator invalid messagecontent",
					Device::Net::ThreatLevel::Uncommon);

				return false;
			}

			return true;
		}
	};
}
