#include "CoreDevice.h"

#include <Operator/source/device/DatabaseDevice.h>
#include <Operator/source/operator/Operator.h>

#include <chrono>
#include <thread>

namespace
{
	Operator::Operator* server;
}

namespace Device::Core
{
	bool Initialize()
	{
		Log::Section(L"Initializing devices");

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

		server = new Operator::Operator();
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
	}

	void Run()
	{
		while (server->getStatus() != Operator::Operator::Shutdown)
		{
			server->process();

			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}
}
