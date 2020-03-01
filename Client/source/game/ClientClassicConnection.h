#pragma once

#include <Client/source/device/NetDevice.h>

#include <Client/source/game/ClientClassicSelectionModule.h>
#include <Client/source/game/ClientClassicSimulationModule.h>

#include <Client/source/game/net/ClassicSelectionMessage.h>
#include <Client/source/game/net/ClassicSimulatorMessage.h>
#include <Client/source/game/net/SimulatorAuthenticationMessage.h>

#include <Operator/source/Common.h>

namespace Game::Net
{
	struct ClassicConnectionInformation
	{
		Operator::ConnectionKey key;
		Operator::UserID userID;
		SteamNetworkingIPAddr address;
	};

	class ClientClassicConnection
		:
		public Device::Net::Client
	{
	public:
		bool initialize(const ClassicConnectionInformation& info)
		{
			return this->connect(info.address);
		}

	protected:
		const ClassicConnectionInformation connectionInfo;

		virtual void onModuleSelection(ClientClassicSelectionModule* const module) = 0;
		virtual void onModuleSimulation(ClientClassicSimulationModule* const module) = 0;

		virtual void onConnectionLost(const int reason) override
		{
			// think about allowing lost connections for 
			// about an hour to stay and reconnect if possible
		}

		virtual void onConnectionClosed(const int reason) override
		{

		}

	private:
		void onMessage(
			const Device::Net::MessageID messageID, 
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{

			}
		}

		void onConnectionOpen() override
		{
			Net::Client::AuthenticationMessageID::Authenticate;


		}

		void onThreatIdentified(
			const Device::Net::MessageID messageID,
			const wchar_t* const note,
			const Device::Net::ThreatLevel level) override
		{

		}
	};
}
