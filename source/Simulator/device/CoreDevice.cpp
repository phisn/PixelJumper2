#include "CoreDevice.h"
#include "game/HostClassicSimulator.h"

#include "Common/EncryptionModule.h"
#include "OperatorClient/OperatorClient.h"
#include "OperatorClient/request/CommonAuthenticationRequest.h"
#include "OperatorClient/request/CommonRegisterClassicHostRequest.h"
#include "ResourceCore/ResourceInterface.h"

namespace
{
	Game::HostClassicSimulator* simulator;
}

namespace Device::Core
{
	bool AuthenticateOperator();
	bool LoadSimulatorResources();
	bool RegisterAsClassicHost();

	bool Initialize()
	{
		if (!Net::Core::Initialize())
		{
			return false;
		}

		SteamNetworkingIPAddr ipAddress;
		ipAddress.SetIPv6LocalHost(9928);
		Operator::Client::Initialize(ipAddress);

		if (!AuthenticateOperator())
		{
			return false;
		}

		if (!Resource::Interface::Initialize())
		{
			return false;
		}

		simulator = new Game::HostClassicSimulator();

		if (!LoadSimulatorResources())
		{
			return false;
		}

		if (!RegisterAsClassicHost())
		{
			return false;
		}

		if (!simulator->initialize())
		{
			return false;
		}

		return true;
	}

	void Uninitialize()
	{
		Operator::Client::Uninitialize();
	}

	void Run()
	{
		sf::Clock clock;
		while (true)
		{
			const sf::Time time = clock.restart();

			Net::Core::Process();
			simulator->onLogic(time);

			if (const sf::Time time = clock.getElapsedTime(); time < simulator->settings.interval)
				sf::sleep(simulator->settings.interval - time);
		}
	}

	bool AuthenticateOperator()
	{
		Net::Client::OperatorAuthenticationMessage* message =
			new Net::Client::OperatorAuthenticationMessage;
		Module::Encryption::HashCommon(
			(unsigned char*) message->content.hash,
			(const unsigned char*) "admin", 5);
		message->username = "admin";

		Operator::CommonAuthenticationRequest request;
		
		const Operator::Client::PushRequestFailure result = Operator::Client::PushRequest(
			Net::Client::OperatorAuthenticationMessageID::Authenticate,
			message,
			&request);

		if (result != Operator::Client::PushRequestFailure::Success)
		{
			Log::Error(L"push request failed", (int)result, L"reason");

			return false;
		}

		return Operator::AwaitSyncRequest(&request);
	}

	bool LoadSimulatorResources()
	{
		Resource::World* const world = new Resource::World;
		if (!Resource::Interface::LoadResource(
				world,
				Resource::ResourceType::World,
				L"world"))
		{
			Log::Error(L"Failed to load resource");

			return false;
		}

		simulator->pushResource(world);
		return true;
	}

	bool RegisterAsClassicHost()
	{
		Net::Client::RegisterClassicHostMessage* message = 
			new Net::Client::RegisterClassicHostMessage;

		message->maxPlayers = 10;
		message->port = 9927;

		Operator::CommonRegisterClassicHostRequest request;

		const Operator::Client::PushRequestFailure result = Operator::Client::PushRequest(
			Net::Client::OperatorCommonMessageID::RegisterClassicHost,
			message,
			&request);

		if (result != Operator::Client::PushRequestFailure::Success)
		{
			Log::Error(L"push request failed", (int)result, L"reason");

			return false;
		}

		return Operator::AwaitSyncRequest(&request);
	}
}
