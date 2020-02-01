#pragma once

#include <Client/source/operator/OperatorAccess.h>

namespace Game::Net
{
	/*
	
		1. Client requests operator
		   auth token
		2. Operator checks credentials
		   and sends auth token
		3. Client hashes auth token
		   with random salt to auth
		4. Client requests auth at
		   simulator
		5. AuthHandler requests operator 
		   confirmation
		6. Operator confirms auth
		   playerid
		7. AuthHandler accept client 
		   auth
		8. Client requests start of 
		   simulation
		9. Simulation accepts request 
		   and starts
	
	*/

	struct AuthenticationMessage
	{
		Operator::ClientIdentifactor identification;
	};

	// authhandler as base for remoteconnection
	class AuthenticationHandler
		:
		public Operator::OperatorAccessClient,
		public Device::Net::ClientHandler
	{
	public:
		enum Status
		{
			Running,
			Authenticating,
			Disconnected,
			Timeout,
			InvalidToken,
			Error
		};

		AuthenticationHandler(const sf::Time timeout)
			:
			timeout(timeout)
		{
		}

		void onLogic(const sf::Time time)
		{
			counter += time;

			if (counter > timeout)
			{
				status = Timeout;
			}
		}

	private:
		Status status;

		const sf::Time timeout;
		sf::Time counter;

		void onMessage(
			const Device::Net::MessageID messageID, 
			Resource::ReadPipe* const pipe) override
		{
		}

		void onOperatorClientRejected() override
		{
			status = InvalidToken;
		}

		void onOperatorClientDisconnected() override
		{
			status = Disconnected;
		}
	};
}
