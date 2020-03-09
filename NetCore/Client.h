#pragma once

#include "ClientHandler.h"

namespace Net
{
	class Client
		:
		public ClientHandler,
		public ClientInterface
	{
	public:
		Client()
			:
			ClientHandler(k_HSteamNetConnection_Invalid)
		{
		}

		virtual bool processMessages() override
		{
			if (connection == k_HSteamNetConnection_Invalid)
			{
				return ClientHandler::processMessages();
			}

			return true;
		}
		
	protected:
		virtual void onConnectionOpened() override
		{
			connection = this->getSocket();
		}
	};
}
