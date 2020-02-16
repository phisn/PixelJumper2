#pragma once

#include <Client/source/operator/OperatorRequest.h>

#include <Operator/source/net/AuthClientMessage.h>
#include <Operator/source/net/AuthOperatorMessage.h>

namespace Operator
{
	class AuthenticationCallback
	{
	public:
		typedef std::function<void(const Operator::UserID)> RequestAcceptCallback;
		typedef std::function<void()> RequestRejectCallback;

		AuthenticationCallback(
			RequestAcceptCallback requestAcceptCallback,
			RequestRejectCallback requestRejectCallback,
			Request::RequestFailCallback requestFailCallback)
			:
			requestAcceptCallback(requestAcceptCallback),
			requestRejectCallback(requestRejectCallback),
			requestFailCallback(requestFailCallback)
		{
		}

	protected:
		RequestAcceptCallback requestAcceptCallback;
		RequestRejectCallback requestRejectCallback;
		Request::RequestFailCallback requestFailCallback;
	};

	class TokenRequest
		:
		public Request
	{
	public:
		bool processEvent(
			const Device::Net::MessageID messageID,
			void* const rawMessage)
		{
			switch (messageID)
			{
			case Operator::Net::Host::AuthMessageID::AcceptToken:
			{
				Operator::Net::Host::AcceptTokenMessage* message =
					(Operator::Net::Host::AcceptTokenMessage*) rawMessage;

				onAccepted(message->userID);
				return true;
			}
			case Operator::Net::Host::AuthMessageID::RejectToken:
				onRejected();

				return true;
			}

			return false;
		}
		
	protected:
		virtual void onAccepted(const Operator::UserID userID) = 0;
		virtual void onRejected() = 0;
	};

	class CommonTokenRequest
		:
		public TokenRequest,
		public AuthenticationCallback
	{
	public:
		using AuthenticationCallback::AuthenticationCallback;

		void onRequestFailed(const Reason reason) override
		{
			requestFailCallback(reason);
		}

	private:
		void onAccepted(const Operator::UserID userID) override
		{
			requestAcceptCallback(userID);
		}

		void onRejected() override
		{
			requestRejectCallback();
		}
	};

	class AuthenticationRequest
		:
		public Request
	{
	public:
		bool processEvent(
			const Device::Net::MessageID messageID,
			void* const rawMessage)
		{
			switch (messageID)
			{
			case Operator::Net::Host::AuthMessageID::AcceptAuthentication:
			{
				Operator::Net::Host::AcceptAuthenticationMessage* message =
					(Operator::Net::Host::AcceptAuthenticationMessage*) rawMessage;

				onAccepted(message->userID);
				return true;
			}
			case Operator::Net::Host::AuthMessageID::RejectAuthentication:
				onRejected();

				return true;
			}

			return false;
		}

	protected:
		virtual void onAccepted(const Operator::UserID userID) = 0;
		virtual void onRejected() = 0;
	};

	class CommonAuthenticationRequest
		:
		public AuthenticationRequest,
		public AuthenticationCallback
	{
	public:
		using AuthenticationCallback::AuthenticationCallback;
		
		void onRequestFailed(const Reason reason) override
		{
			requestFailCallback(reason);
		}

	private:
		void onAccepted(const Operator::UserID userID) override
		{
			requestAcceptCallback(userID);
		}

		void onRejected() override
		{
			requestRejectCallback();
		}
	};

	class RegistrationRequest
		:
		public Request
	{
	public:
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
				return true;
			}
			case Operator::Net::Host::AuthMessageID::RejectRegistration:
			{
				Operator::Net::Host::RejectRegistrationMessage* message =
					(Operator::Net::Host::RejectRegistrationMessage*) rawMessage;

				onRejected(message->reason);
				return true;
			}
			}

			return false;
		}

	protected:
		virtual void onAccepted(const Operator::UserID userID) = 0;
		virtual void onRejected(const Operator::Net::Host::RejectRegistrationMessage::Reason reason) = 0;
	};

	class CommonRegistrationRequest
		:
		public RegistrationRequest
	{
	public:
		typedef std::function<void(const Operator::Net::Host::RejectRegistrationMessage::Reason)> RejectCallback;

		CommonRegistrationRequest(
			const AuthenticationCallback::RequestAcceptCallback acceptCallback,
			const RejectCallback rejectCallback,
			const RequestFailCallback failCallback)
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

		const AuthenticationCallback::RequestAcceptCallback acceptCallback;
		const RejectCallback rejectCallback;
		const RequestFailCallback failCallback;
	};
}
