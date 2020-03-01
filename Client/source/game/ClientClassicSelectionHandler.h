#pragma once

#include <Client/source/game/ClassicSimulation.h>
#include <Client/source/game/net/ClassicSelectionMessage.h>
#include <Client/source/net/RequestHandlerBase.h>

namespace Game::Net
{
	struct ClientClassicSelectionHandlerCallback
	{
		virtual void onSimulationCreated(
			const SimulationBootInformation& info,
			const Host::AcceptSimulationRequestMessage& message) = 0;
	};

	class ClientClassicSelectionHandler
		:
		public ::Net::RequestHandler
	{
	public:
		ClientClassicSelectionHandler(ClientClassicSelectionHandlerCallback* const callback)
			:
			callback(callback)
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
			case Host::ClassicSelectionMessageID::AcceptSimulationRequest:
				if (Host::AcceptSimulationRequestMessage message; loadMessage(messageID, &message, pipe))
				{
					onAcceptSimulationRequest(message);
				}

				return true;
			case Host::ClassicSelectionMessageID::RejectSimulationRequest:
				if (Host::RejectSimulationRequestMessage message; loadMessage(messageID, &message, pipe))
				{
					onRejectSimulationRequest(message);
				}

				return true;
			}

			return false;
		}

	protected:
		void requestSimulation(SimulationBootInformation info)
		{
			Client::PrepareSimulationMessage message;
			message.representationID = info.representationID;
			message.world = info.worldID;

			access->accessSendMessage(
				Client::ClassicSelectionMessageID::PrepareSimulation,
				&message);
		}

		virtual void onSimulationRequestFailed(const Host::RejectSimulationRequestMessage::Reason reason) = 0;

	private:
		ClientClassicSelectionHandlerCallback* const callback;
		SimulationBootInformation currentBootInfo;

		void onAcceptSimulationRequest(const Host::AcceptSimulationRequestMessage& message)
		{
			callback->onSimulationCreated(currentBootInfo, message);
		}

		void onRejectSimulationRequest(const Host::RejectSimulationRequestMessage& message)
		{
			onSimulationRequestFailed(message.reason);
		}
	};
}
