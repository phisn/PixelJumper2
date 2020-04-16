#include "CoreDevice.h"
#include "simulator/HostClassicSimulator.h"
#include "AuthenticationDevice.h"

#include "Common/EncryptionModule.h"
#include "OperatorClient/OperatorClient.h"
#include "OperatorClient/request/CommonAuthenticationRequest.h"
#include "OperatorClient/request/CommonRegisterClassicHostRequest.h"
#include "ResourceCore/ResourceInterface.h"

namespace
{
	Game::HostClassicSimulator* simulator;
	bool running = true;
}

namespace Device::Core
{
	bool LoadSimulatorResources();
	bool RegisterAsClassicHost();

	bool Initialize()
	{
		running = true;

		if (!Net::Core::Initialize())
		{
			return false;
		}

		SteamNetworkingIPAddr ipAddress;
		//ipAddress.SetIPv6LocalHost(9928);
		ipAddress.ParseString("109.230.236.76:9928");
		Operator::Client::Initialize(ipAddress);

		if (!Authentication::Initialize())
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
		while (running)
		{
			const sf::Time time = clock.restart();

			Net::Core::Process();
			Operator::Client::Process(time);
			simulator->onLogic(time);

			if (const sf::Time time = clock.getElapsedTime(); time < simulator->settings.interval)
				sf::sleep(simulator->settings.interval - time);
		}
	}

	void Shutdown()
	{
		running = false;
	}

	bool LoadSimulatorResources()
	{
		Resource::World* const world = new Resource::World;
		if (!Resource::Interface::LoadResource(
				L"world",
				world,
				Resource::WorldResourceDefinition))
		{
			Log::Error(L"Failed to load resource");

			return false;
		}

		simulator->pushResource(world);
		return true;
	}
}
