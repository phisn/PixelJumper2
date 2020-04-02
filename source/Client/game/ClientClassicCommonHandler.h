#pragma once

#include "GameCore/net/ClassicCommonMessage.h"
#include "NetCore/RequestHandler.h"

namespace Game
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
			const Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Net::Host::ClassicCommonMessageID::InitializeClient:


				return true;
			case Net::Host::ClassicCommonMessageID::AddPlayer:

				return true;
			case Net::Host::ClassicCommonMessageID::RemovePlayer:

				return true;
			}

			return false;
		}
	};
}
