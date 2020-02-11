#pragma once

#include <Client/source/device/NetDevice.h>
#include <Operator/source/Common.h>

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
	// poll events instead of anything else?

	bool Initialize();
	void Uninitialize();

	void Process();

	// think about adding connect
	class AuthenticatorAccess
	{
	public:
		enum Status
		{
			// currently connecting to operator
			Connecting,
			// connected to operator but not authenticated
			// timeout if not authenticated in time
			Connected,
			// currently waiting for authentication
			// response
			Authenticating,
			// authenticated and connected to operator
			// ready for requests
			Authenticated,
			// authenticated but not connected to operator
			// connect before sending requests
			Idle,
			// not connected to operator
			// caused by timeout or invalid
			// authentication
			Unconnected,
			// same as unconnected but caused
			// by an unexpected error
			Error
		};

		virtual bool AuthenticateCredentials(
			const char hash[OPERATOR_HASH_SIZE],
			const std::string username) = 0;
		virtual bool AuthenticateToken(const char token[OPERATOR_HASH_SIZE]) = 0;

		Status getStatus() const
		{
			return status;
		}

	protected:
		Status status = Unconnected;
	};

	class AuthenticationEventHandler
	{
	public:
		virtual void initialize(AuthenticatorAccess* const authenticator)
		{
			this->authenticator = authenticator;
		}

		virtual void onConnected() = 0;
		virtual void onDisconnected() = 0;

		virtual void onTokenAccepted() = 0;
		virtual void onAuthenticationAccepted() = 0;
		virtual void onRegistrationAccepted() = 0;

		virtual void onTokenRejected() = 0;
		virtual void onAuthenticationRejected() = 0;
		virtual void onRegistrationRejected() = 0;

		virtual void onTimeout() = 0;
		virtual void onInternalError() = 0;

	protected:
		AuthenticatorAccess* authenticator;
	};

	// if authenticationeventhandler is already placed
	// the old one is replaced
	void PutAuthenticationEventHandler(AuthenticationEventHandler* const eventHandler);
	// if no authenticationeventhandler is placed this
	// function does nothing
	void PopAuthenticationEventHandler();

	AuthenticationEventHandler* getActiveAuthenticationEventHandler();
}
