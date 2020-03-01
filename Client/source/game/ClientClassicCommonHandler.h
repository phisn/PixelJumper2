#pragma once

#include <Client/source/game/net/ClassicCommonMessage.h>
#include <Client/source/net/RequestHandlerBase.h>

namespace Game::Net
{
	class ClientClassicCommonHandler
		:
		public ::Net::RequestHandler
	{
	public:
		void update() override
		{
		}

		bool onMessage(
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Host::ClassicCommonMessageID::InitializeClient:

				return true;
			case Host::ClassicCommonMessageID::AddPlayer:

				return true;
			case Host::ClassicCommonMessageID::RemovePlayer:

				return true;
			}

			return false;
		}
	};
}
