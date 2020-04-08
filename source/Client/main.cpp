#include "device/InputDevice.h"
#include "scene/RootScene.h"

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/ScreenDevice.h"
#include "imgui-sfml/imgui-SFML.h"
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
		//operator_address.SetIPv6LocalHost(9928);
		const bool result = operator_address.ParseString(DEFAULT_OPERATOR_ADDRESS);
		assert(result);
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

	// make new device
	ImGui::SFML::Init(*Device::Screen::GetWindow());
	ImFont* font = ImGui::GetIO().Fonts->AddFontFromFileTTF("resource/static/font.ttf", 28.f);
	ImGui::SFML::UpdateFontTexture();

	Log::Information(L"initialize successfull");
	Framework::Core::PushScene<Scene::RootScene>();

	sf::Clock clock;
	sf::Event event;

	while (true)
	{
		while (Device::Screen::GetWindow()->pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			Framework::Core::ProcessEvent(event);
		}

		const sf::Time delta = clock.restart();

		ImGui::SFML::Update(
			*Device::Screen::GetWindow(), 
			delta);
		ImGui::PushFont(font);
		Framework::Core::ProcessLogic(delta);
		Operator::Client::Process(delta);
		ImGui::PopFont();

		if (!Framework::Core::IsRunning())
		{
			break;
		}

		Device::Screen::GetWindow()->clear();
		Framework::Core::ProcessDraw(Device::Screen::GetWindow());
		ImGui::SFML::Render(*Device::Screen::GetWindow());
		Device::Screen::GetWindow()->display();

		Net::Core::Process();
	}

	Device::Screen::Uninitialize();
	ImGui::SFML::Shutdown();

	return 0;
}

// test change
