#include "CoreDevice.h"

#include "Operator/OperatorCore.h"

#include <chrono>
#include <thread>

namespace
{
	Operator::OperatorCore* server;
}

namespace Device::Core
{
	bool Initialize()
	{
		Log::Section(L"Initializing devices");

		Module::Random::Initialize();

		if (!Operator::OperatorDatabase::Initialize("pj2.db", "threats.db"))
		{
			Log::Error(L"Failed to initialize database");

			return false;
		}

		if (!Net::Core::Initialize())
		{
			Log::Error(L"Failed to initialize net device");

			return false;
		}

		server = new Operator::OperatorCore(Operator::OperatorCore::Settings{ 100, 20 });
		if (!server->initialize(9928))
		{
			Log::Error(L"Failed to initialize server");
			delete server;

			return false;
		}

		return true;
	}

	// does not check for already beeing initialized
	void Uninitialize()
	{
		// server should already be shutdown before
		// uninitialize is called because this will
		// instantly shutdown all connection
		delete server;

		Operator::OperatorDatabase::Uninitialize();
		Net::Core::Uninitialize();
	}

	void Run()
	{
		sf::Time interval = sf::milliseconds(100);
		sf::Clock clock;

		while (server->getStatus() != Operator::OperatorCore::Shutdown)
		{
			clock.restart();

			Net::Core::Process();
			server->process();

			if (const sf::Time time = clock.getElapsedTime(); time < interval)
				sf::sleep(interval - time);
		}
	}
}
