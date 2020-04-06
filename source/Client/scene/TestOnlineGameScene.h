#pragma once

#include "game/ClientClassicConnection.h"

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/ViewChain.h"
#include "OperatorClient/OperatorClient.h"
#include "OperatorClient/request/FindClassicHostRequest.h"
#include "ResourceCore/ResourceInterface.h"

namespace
{
	class TestClientAuthenticationHandler
		:
		public Game::ClientAuthenticationHandler
	{
	public:
		using ClientAuthenticationHandler::ClientAuthenticationHandler;

	private:
		void onAuthenticationRejected(const Net::Host::AuthenticationRejectedMessage::Reason reason) override
		{
			Log::Error(L"Authentication rejected", (int)reason, L"reason");
			Framework::Core::PopScene();
		}

		void onAuthenticationFailed(const Game::ClientAuthenticationFailure reason) override
		{
			Log::Error(L"Authentication failed", (int)reason, L"reason");
			Framework::Core::PopScene();
		}
	};

	class TestClientCommonHandler
		:
		public Game::ClientClassicSessionHandler
	{
	public:
		using ClientClassicSessionHandler::ClientClassicSessionHandler;


	};

	class TestClientSelectionHandler
		:
		public Game::ClientClassicSelectionHandler
	{
	public:
		using ClientClassicSelectionHandler::ClientClassicSelectionHandler;

	private:
		bool selected = false;

		void update() override
		{
			ClientClassicSelectionHandler::update();

			if (!selected)
			{
				Game::SimulationBootInformation info;
				info.worldID = 0xdd0bb0dd;
				info.representationID = 0;

				requestSimulation(info);

				selected = true;
			}
		}

		void onSimulationRequestFailed(const Net::Host::RejectSimulationRequestMessage::Reason reason) override
		{
			Log::Error(L"simulation request failed", (int)reason, L"reason");
			Framework::Core::PopScene();
		}
	};

	class TestClientSimulationHandler
		:
		public Game::ClientClassicSimulationHandler
	{
	public:
		using ClientClassicSimulationHandler::ClientClassicSimulationHandler;

		~TestClientSimulationHandler()
		{
			Log::Error(L"destructor");
			Framework::Core::PopTemporaryScene();
		}

		using ClientClassicSimulationHandler::synchronize;

	private:
		void onSimulationStarted() override
		{
			Log::Information(L"simulation started");
		}

		void onSimulationFailed(const Game::ClassicSimulation::WorldFailure reason) override
		{
			Log::Error(L"Simulation failed", (int)reason, L"reason");
			Framework::Core::PopScene();
		}
	};
}

namespace Scene
{
	class TestClassicClientSimulationSubScene
		:
		public Framework::Scene
	{
	public:
		TestClassicClientSimulationSubScene(
			TestClientSimulationHandler* const handler,
			Device::View* const gameView)
			:
			handler(handler),
			gameView(gameView)
		{
		}

		bool onCreate() override
		{
			Log::Error(L"scene created");


			return true;
		}

		void onEvent(const sf::Event event) override
		{
			if (event.type == sf::Event::KeyPressed &&
				event.key.control && event.key.code == sf::Keyboard::Key::G)
			{
				handler->synchronize();
			}
		}

		void onLogic(const sf::Time time) override
		{
		}

		void onDraw(sf::RenderTarget* const target) override
		{
			gameView->enable(target);
			handler->onDraw(target);
		}

	private:
		TestClientSimulationHandler* const handler;
		Device::View* const gameView;
	};

	class TestClassicClientScene
		:
		public Game::ClientClassicConnection,
		public Framework::Scene,
		public Operator::FindClassicHostRequest
	{
	public:
		TestClassicClientScene()
			:
			ClientClassicConnection(),
			viewChain(1)
		{
		}

		bool onCreate() override
		{
			Resource::World* const world = new Resource::World;
			if (!Resource::Interface::LoadResource(
					world,
					Resource::ResourceType::World,
					L"world"))
			{
				Log::Error(L"Failed to load resource");

				return false;
			}

			worldContainer[world->content.id] = world;

			const Operator::Client::PushRequestFailure result = Operator::Client::PushRequest(
				Net::Client::OperatorCommonMessageID::HostFindClassic,
				NULL,
				this);

			if (result != Operator::Client::PushRequestFailure::Success)
			{
				return false;
			}

			return true;
		}

		void onEvent(const sf::Event event) override
		{
		}

		void onLogic(const sf::Time time) override
		{
			ClientClassicConnection::onLogic(time);
		}

		void onDraw(sf::RenderTarget* const target) override
		{
		}

	private:
		Game::ClassicConnectionInformation connectionInfo;
		Framework::ViewChain viewChain;

		void onRequestFailure(const RequestInterface::Reason reason) override
		{
			Log::Error(L"Find host request failed",
				(int)reason, L"reason");

			Framework::Core::PopScene();
		}

		void onClassicHostFound(Net::Host::HostFindClassicMessage& answer) override
		{
			char buffer[SteamNetworkingIPAddr::k_cchMaxString] = { };
			answer.address.ToString(buffer, SteamNetworkingIPAddr::k_cchMaxString, true);
			Log::Information(L"Got host '" + carrtowstr(buffer) + L"'");

			connectionInfo.address = answer.address;
			connectionInfo.authenticationTimeout = 800; // 20sec
			connectionInfo.key = answer.key;

			if (!ClientClassicConnection::initialize(connectionInfo))
			{
				Log::Error(L"Failed to initialize classic connection");

				Framework::Core::PopScene();
			}
		}

		void onClassicHostFindRejected(Net::Host::HostFindClassicRejectedMessage& answer) override
		{
			Log::Error(L"Find host request rejected",
				(int)answer.type, L"reason");

			Framework::Core::PopScene();
		}

		Game::ClientAuthenticationHandler* createAuthenticationHandler(const Game::ClientAuthenticationHandlerArguments& arguments) override
		{
			return new TestClientAuthenticationHandler(arguments);
		}

		Game::ClientClassicSelectionHandler* createSelectionHandler() override
		{
			return new TestClientSelectionHandler(this);
		}

		Game::ClientClassicSessionHandler* createSessionHandler(const Game::ClientClassicSessionHandlerArguments& arguments) override
		{
			return new TestClientCommonHandler(arguments);
		}

		Game::ClientClassicSimulationHandler* createSimulationHandler(const Game::ClientClassicSimulationHandlerArguments& arguments) override
		{
			Log::Error(L"creating simulation handler");

			TestClientSimulationHandler* const handler = new TestClientSimulationHandler(
				arguments,
				&viewChain.getView(0),
				0);

			Framework::Core::PushTemporaryScene(
				new TestClassicClientSimulationSubScene(handler, &viewChain.getView(0)),
				false);

			return handler;
		}

		void accessOnRequestFailed(
			const Net::MessageID messageID,
			const ::Net::RequestFailure reason) override
		{
			Log::Error(L"access on request failed", (int)reason, L"reason");
			Framework::Core::PopScene();
		}

		void onMessageSendFailed(
			const Net::MessageID messageID,
			const Net::SendFailure reason) override
		{
			Log::Error(L"message send failed", (int)reason, L"reason");
			Framework::Core::PopScene();
		}

		void onConnectionLost(const int reason) override
		{
			// think about allowing lost connections for 
			// about an hour to stay and reconnect if possible
			Log::Error(L"conneciton lost", (int)reason, L"reason");
			Framework::Core::PopScene();
		}

		void onConnectionClosed(const int reason) override
		{
			Log::Error(L"connection closed", (int)reason, L"reason");
			Framework::Core::PopScene();
		}
	};
}
