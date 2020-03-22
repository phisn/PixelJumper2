#pragma once

#include <Client/source/game/net/ClassicSelectionMessage.h>
#include <Client/source/net/RequestHandlerBase.h>

#include <Simulator/source/game/HostClassicSimulationHandler.h>

namespace Game::Net
{
	struct ClassicSelectionHandlerCallback
	{
		virtual void onSimulationCreated(const SimulationBootInformation& info) = 0;
	};

	class ClassicSelectionHandler
		:
		public ::Net::RequestHandler
	{
	public:
		ClassicSelectionHandler(
			ClassicSelectionHandlerCallback* const callback,
			const Resource::ClassicPlayerResource& classicResource,
			const WorldResourceContainer& container)
			:
			callback(callback),
			classicResource(classicResource),
			container(container)
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
				if(Client::PrepareSimulationMessage message; loadMessage(messageID, &message, pipe))
				{
					onPrepareSimulation(message);
				}

				return true;
			case Client::ClassicSelectionMessageID::QuerySimulation:

				return true;
			}

			return false;
		}

	private:
		ClassicSelectionHandlerCallback* const callback;
		const Resource::ClassicPlayerResource& classicResource;
		const WorldResourceContainer& container;

		void onPrepareSimulation(const Client::PrepareSimulationMessage& request)
		{
			if (std::find(
					classicResource.unlockedRepresentations.begin(),
					classicResource.unlockedRepresentations.end(),
				request.representationID)
				== classicResource.unlockedRepresentations.end())
			{
				Host::RejectSimulationRequestMessage message;
				message.reason = message.InvalidRepresentationID;

				access->accessSendMessage(
					Host::ClassicSelectionMessageID::RejectSimulationRequest,
					&message);

				// worldid selection is restricted by client
				// an invalid selected worldid does normally mean
				// a manipulated client
				access->accessOnThreatIdentified(
					Client::ClassicSelectionMessageID::PrepareSimulation,
					L"invalid representationid requested",
					Device::Net::ThreatLevel::Malicious);

				return;
			}

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

				// worldid selection is restricted by client
				// an invalid selected worldid does normally mean
				// a manipulated client
				access->accessOnThreatIdentified(
					Client::ClassicSelectionMessageID::PrepareSimulation,
					L"invalid worldid requested",
					Device::Net::ThreatLevel::Malicious);

				return;
			}

			WorldResourceContainer::const_iterator world = container.find(request.world);

			if (world == container.cend())
			{
				missingWorldResource();
				return;
			}

			Host::AcceptSimulationRequestMessage message;
			message.initialWorlds.push_back(world->second);

			for (Resource::WorldId worldID : message.initialWorlds[0]->targets)
			{
				world = container.find(worldID);

				if (world == container.cend())
				{
					missingWorldResource();
					return;
				}

				message.initialWorlds.push_back(world->second);
			}

			access->accessSendMessage(
				Host::ClassicSelectionMessageID::AcceptSimulationRequest,
				&message);

			SimulationBootInformation info;
			info.worldID = request.world;
			info.representationID = request.representationID;

			callback->onSimulationCreated(info);
		}

		// here only called by simulation request message
		void missingWorldResource()
		{
			Host::RejectSimulationRequestMessage message;
			message.reason = message.InternalResourceMissing;

			access->accessSendMessage(
				Host::ClassicSelectionMessageID::RejectSimulationRequest,
				&message);
			access->accessOnRequestFailed(
				Client::ClassicSelectionMessageID::PrepareSimulation,
				::Net::RequestFailure::Internal);
		}
	};
}
