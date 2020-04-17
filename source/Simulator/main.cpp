#include "device/CoreDevice.h"
#include "device/AuthenticationDevice.h"

#include "Logger/Logger.h"

#include <iostream>

int main(int argc, char** argv)
{
	Log::Output::Add(Log::Output::CONSOLE_OUT, Log::Level::Warning);

	if (argc == 2)
	{
		std::string argument = argv[1];

		if (argument != "token")
		{
			std::cout << "Usage: " << argv[0] << " [\"token\"]" << std::endl;
			return 3;
		}

		Device::Authentication::AquireToken();

		return 2;
	}

	if (argc != 1)
	{
		std::cout << "invalid number of arguments" << std::endl;
		return 1;
	}

	if (Device::Core::Initialize())
	{
		Device::Core::Run();
		Device::Core::Uninitialize();

		return 0;
	}
	else
	{
		return 1;
	}
}
