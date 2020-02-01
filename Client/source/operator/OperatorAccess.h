#pragma once

#include <Client/source/device/NetDevice.h>
#include <Client/source/resource/PlayerResource.h>

#include <openssl/sha.h>

namespace Operator
{
	bool Initialize();
	void Uninitialize();

	class OperatorAccess
	{
	};

	// only if connected
	OperatorAccess* Get();

	bool IsAvailable();
	bool Reconnect();
	
	struct OperatorAccessClient
	{
		// client token was accepted and client is valid
		// called with playerID to prevent playerID spoofing
		virtual void onOperatorClientConnected(const Resource::PlayerID playerID) = 0;

		// client token was rejected. potential identifier
		// for cheating but could also be a disconnection while
		// validating
		virtual void onOperatorClientRejected() = 0;
		
		// client disconnected from operator and the token is
		// no longer valid. client should be kicked if not already
		// done
		virtual void onOperatorClientDisconnected() = 0;
	};

	struct ClientIdentifactor
	{
		char token[SHA_DIGEST_LENGTH];
		char salt[16];
	};

	// identificate a client as a server
	// used to prove a client real
	void RegisterClient(
		ClientIdentifactor* const identificator,
		OperatorAccessClient* const client);

	struct AuthenticationHandler
		:
		public ISteamNetworkingSocketsCallbacks,
		public OperatorAccessClient
	{
		/*
		
		onOperatorClientConnected:
			now create remoteconnection
		
		*/

		void onClientIdentifiactorReceived(ClientIdentifactor identificator)
		{
			RegisterClient(&identificator, this);
		}
	};


	std::future<sf::Uint64> CreateAuthentication();
	bool ValidateAuthentication();
}
