#pragma once

#include "HostClassicSimulationHandler.h"

#include "GameCore/net/ClassicSelectionMessage.h"
#include "NetCore/RequestHandler.h"

namespace Game
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
			const ::Net::MessageID messageID, 
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case ::Net::Client::ClassicSelectionMessageID::PrepareSimulation:
				if(::Net::Client::PrepareSimulationMessage message; loadMessage(messageID, &message, pipe))
				{
					onPrepareSimulation(message);
				}

				return true;
			case ::Net::Client::ClassicSelectionMessageID::QuerySimulation:

				return true;
			}

			return false;
		}

	private:
		ClassicSelectionHandlerCallback* const callback;
		const Resource::ClassicPlayerResource& classicResource;
		const WorldResourceContainer& container;

		void onPrepareSimulation(const ::Net::Client::PrepareSimulationMessage& request)
		{
			if (std::find(
					classicResource.unlockedRepresentations.begin(),
					classicResource.unlockedRepresentations.end(),
				request.content.representationID)
				== classicResource.unlockedRepresentations.end())
			{
				::Net::Host::RejectSimulationRequestMessage message;
				message.content.reason = message.content.InvalidRepresentationID;

				access->sendMessage(
					::Net::Host::ClassicSelectionMessageID::RejectSimulationRequest,
					&message);

				// worldid selection is restricted by client
				// an invalid selected worldid does normally mean
				// a manipulated client
				access->onThreatIdentified(
					::Net::Client::ClassicSelectionMessageID::PrepareSimulation,
					L"invalid representationid requested",
					::Net::ThreatLevel::Malicious);

				return;
			}

			if (std::find(
					classicResource.unlockedWorlds.begin(),
					classicResource.unlockedWorlds.end(),
				request.content.world)
				== classicResource.unlockedWorlds.end())
			{
				::Net::Host::RejectSimulationRequestMessage message;
				message.content.reason = message.content.InvalidWorldID;

				access->sendMessage(
					::Net::Host::ClassicSelectionMessageID::RejectSimulationRequest,
					&message);

				// worldid selection is restricted by client
				// an invalid selected worldid does normally mean
				// a manipulated client
				access->onThreatIdentified(
					::Net::Client::ClassicSelectionMessageID::PrepareSimulation,
					L"invalid worldid requested",
					::Net::ThreatLevel::Malicious);

				return;
			}

			WorldResourceContainer::const_iterator world = container.find(request.content.world);

			if (world == container.cend())
			{
				missingWorldResource();
				return;
			}

			::Net::Host::AcceptSimulationRequestMessage message;
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

			access->sendMessage(
				::Net::Host::ClassicSelectionMessageID::AcceptSimulationRequest,
				&message);

			SimulationBootInformation info;
			info.worldID = request.content.world;
			info.representationID = request.content.representationID;

			callback->onSimulationCreated(info);
		}

		// here only called by simulation request message
		void missingWorldResource()
		{
			::Net::Host::RejectSimulationRequestMessage message;
			message.content.reason = message.content.InternalResourceMissing;

			access->sendMessage(
				::Net::Host::ClassicSelectionMessageID::RejectSimulationRequest,
				&message);
			access->accessOnRequestFailed(
				::Net::Client::ClassicSelectionMessageID::PrepareSimulation,
				::Net::RequestFailure::Internal);
		}
	};
}
