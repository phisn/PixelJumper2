#include <Client/source/device/CoreDevice.h>
#include <Client/source/logger/Logger.h>

#include <iostream>

#ifdef _DEBUG or not _WIN32
	int main()
#else
	#include <Windows.h>
	int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{
#ifdef _DEBUG
	Log::Output::Add(Log::Output::CONSOLE_OUT, Log::Level::Information);
#else
	Log::Output::Add(Log::Output::CONSOLE_OUT, Log::Level::Warning);
#endif
	Log::Information(L"Initializing game");

	const Device::Core::Error error = Device::Core::Initialize();

	if (error != Device::Core::Error::Success)
	{
		std::cin.ignore();

		return (int) error;
	}
	
	Log::Information(L"Starting game");
	return Device::Core::RunGameLoop();
}
