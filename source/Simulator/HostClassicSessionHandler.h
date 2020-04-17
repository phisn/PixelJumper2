#pragma once

#include "GameCore/net/ClassicSessionMessage.h"
#include "NetCore/RequestHandler.h"
#include "ResourceCore/ClassicContextResource.h"

namespace Game
{
	class HostClassicSessionHandler
		:
		public ::Net::RequestHandler
	{
	public:
		HostClassicSessionHandler(
			Resource::ClassicPlayerResource& player,
			const Resource::ClassicContextResource& classicContext)
			:
			classicContext(classicContext)
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
			case Net::Client::ClassicSessionMessageID::InitiateClassicUser:
				// not needed anymore?

				break;

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

	private:
		const Resource::ClassicContextResource& classicContext;
	};
}
