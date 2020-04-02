#pragma once

#include "GameCore/ClassicSimulation.h"
#include "GameCore/net/ClassicSelectionMessage.h"
#include "NetCore/RequestHandler.h"


namespace Game
{
	struct ClientClassicSelectionHandlerCallback
	{
		virtual void onSimulationCreated(
			const SimulationBootInformation& info,
			const Net::Host::AcceptSimulationRequestMessage& message) = 0;
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
			const Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Net::Host::ClassicSelectionMessageID::AcceptSimulationRequest:
				if (Net::Host::AcceptSimulationRequestMessage message; loadMessage(messageID, &message, pipe))
				{
					onAcceptSimulationRequest(message);
				}

				return true;
			case Net::Host::ClassicSelectionMessageID::RejectSimulationRequest:
				if (Net::Host::RejectSimulationRequestMessage message; loadMessage(messageID, &message, pipe))
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
			Net::Client::PrepareSimulationMessage message;

			message.representationID = info.representationID;
			message.world = info.worldID;
			currentBootInfo = info;

			access->sendMessage(
				Net::Client::ClassicSelectionMessageID::PrepareSimulation,
				&message);
		}

		virtual void onSimulationRequestFailed(const Net::Host::RejectSimulationRequestMessage::Reason reason) = 0;

	private:
		ClientClassicSelectionHandlerCallback* const callback;
		SimulationBootInformation currentBootInfo;

		void onAcceptSimulationRequest(const Net::Host::AcceptSimulationRequestMessage& message)
		{
			callback->onSimulationCreated(currentBootInfo, message);
		}

		void onRejectSimulationRequest(const Net::Host::RejectSimulationRequestMessage& message)
		{
			onSimulationRequestFailed(message.reason);
		}
	};
}
