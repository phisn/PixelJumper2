#include "device/InputDevice.h"
#include "scene/TestGameScene.h"

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/ScreenDevice.h"
#include "NetCore/NetCore.h"
#include "OperatorClient/OperatorClient.h"
#include "ResourceCore/ResourceInterface.h"

#include <iostream>

#define DEFAULT_OPERATOR_ADDRESS "109.230.236.76:9928"

void usage(char** argv)
{
	Log::Error(L"usage: " + carrtowstr(argv[0]) + L" [operator ipaddress]");
}

int main(int argc, char** argv)
{
	Log::Output::Add(Log::Output::CONSOLE_OUT, Log::Level::Information);

#ifdef _DEBUG
	struct AwaitClose
	{
		~AwaitClose()
		{
			Log::Information(L"press enter to close ...");
			std::cin.ignore();
			std::cin.ignore();
		}

	} awaitClose;
#endif

	SteamNetworkingIPAddr operator_address;

	if (argc > 2)
	{
		usage(argv);
		return 1;
	}

	if (argc == 2)
	{
		if (!operator_address.ParseString(argv[1]))
		{
			Log::Error(L"got invalid ipaddress", carrtowstr(argv[1]), L"input");
			usage(argv);
			return 2;
		}
	}
	else
	{
		assert(operator_address.ParseString(DEFAULT_OPERATOR_ADDRESS));
	}

	if (!Resource::Interface::Initialize())
	{
		return 3;
	}

	if (!Net::Core::Initialize())
	{
		return 4;
	}

	if (!Framework::Core::Initialize())
	{
		return 4;
	}

	Operator::Client::Initialize(operator_address);

	if (!Device::Screen::Initialize())
	{
		return 5;
	}

	if (!Device::Input::Initialize())
	{
		return 7;
	}

	Log::Information(L"initialize successfull");

	if (!Framework::Core::PushScene<Scene::TestGameScene>())
	{
		Log::Error(L"push main scene failed");
		return 6;
	}

	sf::Clock clock;
	sf::Event event;

	while (true)
	{
		const sf::Time delta = clock.restart();
		Framework::Core::ProcessLogic(delta);
		Operator::Client::Process(delta);

		if (!Framework::Core::IsRunning())
		{
			break;
		}

		while (Device::Screen::GetWindow()->pollEvent(event))
		{
			Framework::Core::ProcessEvent(event);
		}

		Device::Screen::GetWindow()->clear();
		Framework::Core::ProcessDraw(Device::Screen::GetWindow());
		Device::Screen::GetWindow()->display();

		Net::Core::Process();
	}

	return 0;
}
