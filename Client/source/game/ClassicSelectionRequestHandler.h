#pragma once

#include <Client/source/game/net/ClassicSelectionMessage.h>
#include <Client/source/net/RequestHandlerBase.h>

namespace Game::Net
{
	class ClassicSelectionRequestHandler
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
			case Client::ClassicSelectionMessageID::PrepareSimulation:


				break;
			case Client::ClassicSelectionMessageID::QuerySimulation:


				break;
			}
		}

	private:
	};
}
