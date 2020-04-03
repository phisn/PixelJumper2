#pragma once

#include "ClientHandler.h"

namespace Net
{
	class ClientBase
		:
		public ClientHandler,
		public ClientInterface
	{
	public:
		ClientBase()
			:
			ClientHandler(k_HSteamNetConnection_Invalid)
		{
		}

		virtual void processMessages() override
		{
			if (connection != k_HSteamNetConnection_Invalid)
			{
				ClientHandler::processMessages();
			}
		}
		
	protected:
		virtual void onConnectionOpened() override
		{
			connection = this->getSocket();
		}
	};
}
