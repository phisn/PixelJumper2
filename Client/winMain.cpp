#include <iostream>

#include <Client/source/device/CoreDevice.h>
#include <Client/source/logger/Logger.h>

#include <Client/source/device/NetDevice.h>

#include <steam/steamnetworkingsockets.h>

#if defined(_DEBUG) || !defined(_WIN32)
	int main()
#else
	#include <Windows.h>
	int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{
#ifdef _DEBUG
	Log::Output::Add(Log::Output::CONSOLE_OUT, Log::Level::Information);
#else
	Log::Output::Add(Log::Output::FILE_OUT, Log::Level::Warning);
#endif
	const Device::Core::Error error = Device::Core::Initialize();
/*
	Log::Information(Device::Net::Initialize(12345), L"result");
	class C : public Device::Net::Client
	{
	public:
		C()
			:
			Client(
				Device::Net::Client::Settings{ },
				Device::Net::Target{ 12345, sf::IpAddress::LocalHost })
		{
		}
		
	private:
		void onMessage(Device::Net::MessageRead* const message) override
		{
			Log::Information(L"Message");
		}

	} client;
*/

	if (error != Device::Core::Error::Success)
	{
		std::cin.ignore();

		return (int) error;
	}
	
	Log::Information(L"Starting game");
	int result = Device::Core::RunGameLoop();
	Device::Core::Uninitialize();

	std::cin.ignore();
	return result;
}
