#pragma once

#include <Client/source/device/NetDevice.h>
#include <Client/source/resource/PlayerResource.h>

#include <openssl/sha.h>

#include <future>

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
	bool Initialize();
	void Uninitialize();

	struct AuthenticationEventHandler
	{
		virtual void onConnected() = 0;
		virtual void onDisconnected() = 0;
		
		virtual void onTokenRejected() = 0;
		virtual void onAuthenticationRejected() = 0;
		virtual void onRegistrationRejected() = 0;

		virtual void onTokenRejected() = 0;
		virtual void onAuthenticationRejected() = 0;
		virtual void onREgistrationRejected() = 0;

		virtual void onTimeout() = 0;
		virtual void onInternalError() = 0;
	};

	void ProcessAuthenticationHandler(AuthenticationEventHandler* const handler);

	void AuthenticateCredentials(
		const char hash[OPERATOR_HASH_SIZE],
		const std::string username);
	void AuthenticateToken(const char token[OPERATOR_HASH_SIZE]);

	enum class Status
	{
		Connecting,
		Connected,
		Unconnected
	};

	const Status getStatus();

	/*
	typedef sf::Uint32 RequestID;

	enum class RequestType : sf::Uint8
	{
		AuthenticationToken,
		PlayerData
	};
	*/
}
