#include "Logger/Logger.h"
#include "device/CoreDevice.h"

int main()
{
	Log::Output::Add(Log::Output::CONSOLE_OUT, Log::Level::Warning);

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
