#include "CoreDevice.h"

#include <Client/source/device/NetDevice.h>
#include <Client/source/device/RandomDevice.h>

#include <Operator/source/device/DatabaseDevice.h>
#include <Operator/source/operator/Operator.h>

#include <chrono>
#include <thread>

namespace
{
	Operator::Net::Operator* server;
}

namespace Device::Core
{
	bool Initialize()
	{
		Log::Section(L"Initializing devices");

		Random::Initialize();

		if (!Database::Initialize())
		{
			Log::Error(L"Failed to initialize database");

			return false;
		}

		if (!Net::Initialize())
		{
			Log::Error(L"Failed to initialize net device");

			return false;
		}

		server = new Operator::Net::Operator(Operator::Net::Operator::Settings{ 100, 20 });
		if (!server->initialize())
		{
			Log::Error(L"Failed to initialize server");
			delete server;

			return false;
		}
	}

	// does not check for already beeing initialized
	void Uninitialize()
	{
		// server should already be shutdown before
		// uninitialize is called because this will
		// instantly shutdown all connection
		delete server;

		Net::Uninitialize();
		Database::Unintialize();
	}

	void Run()
	{
		sf::Time interval = sf::milliseconds(100);
		sf::Clock clock;

		while (server->getStatus() != Operator::Net::Operator::Shutdown)
		{
			server->process();

			if (const sf::Time time = clock.restart(); time < interval)
				sf::sleep(interval - time);
		}
	}
}
