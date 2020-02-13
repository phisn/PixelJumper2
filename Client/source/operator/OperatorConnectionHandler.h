#pragma once

#include <Client/source/device/NetDevice.h>

#include <Operator/source/net/AuthClientMessage.h>
#include <Operator/source/net/AuthOperatorMessage.h>

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
	struct Request
	{
		enum class Reason
		{
			ConnectionClosed,
			ConnectionLost,
			OperatorInternalError,
			OperatorTimeout,
			Timeout
		};

		virtual void onRequestFailed(const Reason reason) = 0;

		// return is finished request
		virtual bool processEvent(
			const Device::Net::MessageID messageID,
			void* const message) = 0;

		typedef void (FailCallback)(const Reason reason);
	};

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

			Request* const request;

		private:
			const sf::Time timeout;
			sf::Time time;
		};

		ConnectionHandler()
		{
		}

		static ConnectionHandler* instance;

	public:
		// used when already been authenticated and
		// want to reauthenticate when status is idle
		// else onAuthenticated is called as soon as
		// possible. inherited by other requests and
		// called
		class ReauthenticateRequest
		{
		public:
			virtual void onAuthenticated() = 0;
			virtual void onRejected() = 0;

			// needed inside connectionhandler
			// because of status == idle and authtoken
		};

		static bool Initialize(const SteamNetworkingIPAddr ipAddress)
		{
			instance = new ConnectionHandler();

			if (!instance->connect(ipAddress))
			{
				delete instance;
				return false;
			}

			return true;
		}

		static void Uninitialize()
		{
			if (instance)
				delete instance;
		}

		static bool PushRequest(
			const Device::Net::MessageID messageID,
			Game::Net::NetworkMessage* const message,
			Request* const request)
		{
			if (!instance->sendCommonMessage(messageID, message))
			{
				return false;
			}

			instance->requests.emplace_back(request, sf::seconds(20));

			return true;
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

		Status getStatus()
		{
			return status;
		}
		
	private:
		bool authenticated = false;
		Status status = Status::Unconnected;

		UserID userID;
		AuthenticationToken token;

		std::vector<RequestContainer> requests;

		void onMessage(
			const Device::Net::MessageID messageID, 
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Operator::Net::Host::AuthMessageID::AcceptRegistration:
			{
				Operator::Net::Host::AcceptRegistrationMessage message;

				if (loadCommonMessage(messageID, &message, pipe))
				{
					break;
				}

				onAcceptRegistration(message);
				processRequests(messageID, (void*) &message);
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
				processRequests(messageID, (void*) &message);
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
				processRequests(messageID, (void*) &message);
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
				processRequests(messageID, (void*) &message);
				break;
			}
			case Operator::Net::Host::AuthMessageID::RejectAuthentication:
				processRequests(messageID, NULL);

				break;
			case Operator::Net::Host::AuthMessageID::Timeout:
				processRequests(messageID, NULL);
				failRequests(Request::Reason::OperatorTimeout);

				break;
			case Operator::Net::Host::AuthMessageID::InternalError:
				processRequests(messageID, NULL);
				status = Status::Error;
				failRequests(Request::Reason::OperatorInternalError);

				break;
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
			authenticated = true;
		}

		void failRequests(const Request::Reason reason)
		{
			for (RequestContainer& request : requests)
				request.request->onRequestFailed(reason);

			requests.clear();
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
					requests.erase(iterator);
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

	class RegistrationRequest
		:
		public Request
	{
	public:
		virtual void onAccepted(const Operator::UserID userID) = 0;
		virtual void onRejected(const Operator::Net::Host::RejectRegistrationMessage::Reason reason) = 0;

		bool processEvent(
			const Device::Net::MessageID messageID,
			void* const rawMessage)
		{
			switch (messageID)
			{
			case Operator::Net::Host::AuthMessageID::AcceptRegistration:
			{
				Operator::Net::Host::AcceptRegistrationMessage* message =
					(Operator::Net::Host::AcceptRegistrationMessage*) rawMessage;

				onAccepted(message->userID);
				break;
			}
			case Operator::Net::Host::AuthMessageID::RejectRegistration:
			{
				Operator::Net::Host::RejectRegistrationMessage* message =
					(Operator::Net::Host::RejectRegistrationMessage*) rawMessage;

				onAccepted(message->reason);
				break;
			}
			}
		}
	};

	class CommonRegistrationRequest
		:
		public RegistrationRequest
	{
	public:
		typedef std::function<void(const Operator::UserID)> AcceptCallback;
		typedef std::function<void(const Operator::Net::Host::RejectRegistrationMessage::Reason)> RejectCallback;
		typedef std::function<FailCallback> FullFailCallback;

		CommonRegistrationRequest(
			const AcceptCallback acceptCallback,
			const RejectCallback rejectCallback,
			const FullFailCallback failCallback)
			:
			acceptCallback(acceptCallback),
			rejectCallback(rejectCallback),
			failCallback(failCallback)
		{
		}

		void onRequestFailed(const Reason reason) override
		{
			failCallback(reason);
		}

	private:
		void onAccepted(const Operator::UserID userID) override
		{
			acceptCallback(userID);
		}

		void onRejected(const Operator::Net::Host::RejectRegistrationMessage::Reason reason) override
		{
			rejectCallback(reason);
		}

		const AcceptCallback acceptCallback;
		const RejectCallback rejectCallback;
		const FullFailCallback failCallback;
	};
}
