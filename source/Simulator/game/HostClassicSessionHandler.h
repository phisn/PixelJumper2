#pragma once

#include "GameCore/net/ClassicSessionMessage.h"
#include "NetCore/RequestHandler.h"

namespace Game
{
	class HostClassicSessionHandler
		:
		public ::Net::RequestHandler
	{
	public:
		HostClassicSessionHandler()
		{
		}

		void update() override
		{
		}

		bool onMessage(
			const ::Net::MessageID messageID, 
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
				// chat messages
				// allow only one message per update
				// messages should have a ID to be
				// able to remove them
				// need somehow to send message to all
				// other clients
				// some messaging system?
				// maybe something bigger to allow
				// friends, private messages and similar
			}

			return false;
		}
	};
}
