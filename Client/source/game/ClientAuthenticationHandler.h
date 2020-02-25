#pragma once

#include <Client/source/device/NetDevice.h>
#include <Client/source/operator/OperatorConnectionHandler.h>
#include <Client/source/operator/ConnectionKeyRequest.h>

namespace Game::Net
{
	class OfficalClientAuthenticationHandler
		:
		/*
			hides:
			-	connect
			-	process
		*/
		protected Device::Net::Client,
		protected Operator::ConnectionKeyRequest
	{
	public:
		~OfficalClientAuthenticationHandler()
		{
			cancel();
		}

		bool connect(const Operator::UserID userID)
		{
			if (!Operator::ConnectionHandler::IsAuthenticated())
			{
				return false;
			}

			Operator::Net::Client::RequestConnectionKeyMessage* message = 
				new Operator::Net::Client::RequestConnectionKeyMessage;
			message->userID = userID;

			return Operator::ConnectionHandler::PushRequest(
				Operator::Net::Client::RequestMessageID::RequestConnectionKey,
				message,
				(Operator::ConnectionKeyRequest*) this);
		}

		void cancel()
		{
			// removes all possible requests instead of logging them because
			// cancel is rare and the overhead is extremly small
			Operator::ConnectionHandler::PopRequest((Operator::ConnectionKeyRequest*) this);
		}

	private:
		void onConnectionKeyRetrieved()
		{
		}

		void onConnectionOpen() override
		{
		}

		void onConnectionLost(const int reason) override
		{
		}

		void onConnectionClosed(const int reason) override
		{
		}
	};
}
