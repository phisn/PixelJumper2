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
		ClassicSelectionRequestHandler(const Resource::ClassicPlayerResource& classicResource)
			:
			classicResource(classicResource)
		{
		}

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
		const Resource::ClassicPlayerResource& classicResource;

		void onPrepareSimulation(const Client::PrepareSimulationMessage& request)
		{
			if (std::find(
					classicResource.unlockedWorlds.begin(),
					classicResource.unlockedWorlds.end(),
				request.world)
				== classicResource.unlockedWorlds.end())
			{
				Host::RejectSimulationRequestMessage message;
				message.reason = message.InvalidWorldID;

				access->accessSendMessage(
					Host::ClassicSelectionMessageID::RejectSimulationRequest,
					&message);
			}
			else
			{

			}
		}
	};
}
