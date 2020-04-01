#include "CoreDevice.h"

#include "Common/EncryptionModule.h"
#include "OperatorClient/OperatorClient.h"
#include "OperatorClient/request/AuthenticationRequest.h"
#include "ResourceCore/ResourceInterface.h"
#include "game/HostClassicSimulator.h"

namespace
{
	Game::Net::HostClassicSimulator* simulator;
}

namespace Device::Core
{
	bool AuthenticateOperator();
	bool LoadSimulatorResources();

	bool Initialize()
	{
		if (!Device::Net::Initialize())
		{
			return false;
		}

		SteamNetworkingIPAddr ipAddress;
		ipAddress.SetIPv6LocalHost(9928);
		Operator::ConnectionHandler::Initialize(ipAddress);

		if (!AuthenticateOperator())
		{
			return false;
		}

		if (!Resource::Interface::Initialize())
		{
			return false;
		}

		simulator = new Game::Net::HostClassicSimulator();

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
		Operator::ConnectionHandler::Uninitialize();
	}

	void Run()
	{
		sf::Clock clock;
		while (true)
		{
			const sf::Time time = clock.restart();

			Device::Net::Process();
			simulator->onLogic(time);

			if (const sf::Time time = clock.getElapsedTime(); time < simulator->settings.interval)
				sf::sleep(simulator->settings.interval - time);
		}
	}

	bool AuthenticateOperator()
	{
		Operator::Net::Client::AuthenticationMessage message;
		Device::Encryption::HashCommon(
			(unsigned char*)message.content.hash,
			(const unsigned char*) "admin", 5);
		message.username = "admin";

		bool waiting = true, success = false;
		Operator::CommonAuthenticationRequest request(
			[&waiting, &success](const Operator::UserID user)
			{
				waiting = false;
				success = true;
				Log::Information(L"accepted", user, L"user");
			},
			[&waiting]()
			{
				waiting = false;
				Log::Information(L"rejected");
			},
				[&waiting](const Operator::Request::Reason reason)
			{
				waiting = false;
				Log::Information(L"failed", (int)reason, L"reason");
			});

		if (!Operator::ConnectionHandler::PushRequest(
			Operator::Net::Client::AuthMessageID::Authenticate,
			&message,
			&request))
		{
			return false;
		}

		sf::Clock clock;
		while (waiting)
		{
			Operator::ConnectionHandler::Process(clock.restart());
			Device::Net::Process();
			sf::sleep(sf::milliseconds(50));
		}

		return success;
	}

	bool LoadSimulatorResources()
	{
		Resource::World* const world = new Resource::World;
		if (!Resource::Interface::LoadResource(
			world,
			Resource::ResourceType::World,
			L"right"))
		{
			Log::Error(L"Failed to load resource");

			return false;
		}

		simulator->pushResource(world);
		return true;
	}
}
