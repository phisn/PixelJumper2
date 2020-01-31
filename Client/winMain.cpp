#include <iostream>

#include <Client/source/device/CoreDevice.h>
#include <Client/source/logger/Logger.h>

#include <Client/source/device/NetDevice.h>

void doServer()
{
	class _ClientHandler
		:
		public Device::Net::ClientHandler
	{
	public:
		ClientHandler::ClientHandler;

		void onMessage(Resource::ReadPipe* const pipe)
		{
			std::wstring output;
			pipe->readString(&output);
			Log::Information(output);

			output = L"pong";
			beginMessage()->writeString(&output);
			sendMessage();
		}
	};

	class Server
		:
		public Device::Net::Server
	{
	public:
		void process()
		{
			handler->process();
			Device::Net::Server::process();
		}

	private:
		bool askClientConnect(SteamNetworkingIPAddr* const ipAddress)
		{
			Log::Information(L"Client tries to connect");

			return true;
		}

		void onClientConnect(const HSteamNetConnection connection)
		{
			Log::Information(L"Client connected");
			handler = new _ClientHandler(connection);
		}

		void onClientDisconnected(const HSteamNetConnection connection)
		{
			Log::Information(L"Client disconnected");
		}

		void onClientLost(const HSteamNetConnection connection)
		{
			Log::Information(L"Connection lost");
		}

	private:
		_ClientHandler* handler;
	};

	Server server;

	server.Initialize();
	
	while (true)
	{
		server.process();
		Sleep(50);
	}
}

void doClient()
{
	class Client
		:
		public Device::Net::Client
	{
	public:
		void onMessage(Resource::ReadPipe* const pipe)
		{
			std::wstring output;
			pipe->readString(&output);
			Log::Information(output);
		}

	private:
		void onConnectionOpen()
		{
			Log::Information(L"Connection open");

			std::wstring output = L"ping";
			this->beginMessage()->writeString(&output);
			sendMessage();
		}

		void onConnectionLost(const int reason)
		{
			Log::Information(L"Connection lost",
				reason, L"reason");
		}

		void onConnectionClosed(const int reason)
		{
			Log::Information(L"Connection lost",
				reason, L"reason");
		}
	};

	SteamNetworkingIPAddr address;
	address.SetIPv6LocalHost(DEV_NET_PORT);

	Client client;
	client.connect(address);
	
	while (true) 
	{
		client.process();
		Sleep(50);
	}
}


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
	// const Device::Core::Error error = Device::Core::Initialize();
	Device::Net::Initialize();
	int i;
	std::cin >> i;
	if (i == 0)
	{
		doClient();
	}
	else
	{
		doServer();
	}

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
	*/
}
