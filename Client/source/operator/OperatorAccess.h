#pragma once

#include <Client/source/device/NetDevice.h>
#include <Client/source/resource/PlayerResource.h>

#include <openssl/sha.h>

#include <future>

namespace Operator
{
	bool Initialize();
	void Uninitialize();

	struct OperatorHandler
	{
		virtual void onConnected();
		virtual void onDisconnected();

		virtual void onMessage();
	};

	void ProcessOperatorHandler(const OperatorHandler* const handler);

	bool IsConnected();
	bool Reconnect();

	typedef sf::Uint32 RequestID;

	enum class RequestType : sf::Uint8
	{
		AuthenticationToken,
		PlayerData
	};

	struct AuthenticationToken
	{
		unsigned char token[20];
	};

	struct RequestHandler
	{
		enum Response
		{
			Done,
			Denied,
			Error
		};

		virtual void handle(
			const Response response,
			const RequestID requestID,
			const RequestType requestType,
			Resource::ReadPipe* const pipe) = 0;
	};

	const RequestID Request(RequestType request, RequestHandler* const handler);
	
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
		AuthenticationToken token;
		unsigned char salt[16];
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
