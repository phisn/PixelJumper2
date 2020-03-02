#pragma once

#include <Client/source/game/ClientClassicConnection.h>
#include <Client/source/operator/OperatorConnectionHandler.h>
#include <Client/source/operator/request/CommonRequest.h>

namespace
{
	class TestClientAuthenticationHandler
		:
		public Game::Net::ClientAuthenticationHandler
	{
	public:
		using ClientAuthenticationHandler::ClientAuthenticationHandler;

	private:
		void onAuthenticationRejected(const Game::Net::Host::AuthenticationRejectedMessage::Reason reason) override
		{
			Framework::Context::Pop();
		}

		void onAuthenticationFailed(const Game::Net::ClientAuthenticationFailure reason) override
		{
			Framework::Context::Pop();
		}
	};

	class TestClientCommonHandler
		:
		public Game::Net::ClientClassicCommonHandler
	{
	public:
		using ClientClassicCommonHandler::ClientClassicCommonHandler;
	};

	class TestClientSelectionHandler
		:
		public Game::Net::ClientClassicSelectionHandler
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
				info.worldID;
				info.representationID = 0;

				requestSimulation(info);
				
				selected = true;
			}
		}

		void onSimulationRequestFailed(const Game::Net::Host::RejectSimulationRequestMessage::Reason reason) override
		{
			Framework::Context::Pop();
		}
	};

	class TestClientSimulationHandler
		:
		public Game::Net::ClientClassicSimulationHandler
	{
	public:
		using ClientClassicSimulationHandler::ClientClassicSimulationHandler;

		~TestClientSimulationHandler()
		{
			Framework::Context::PopScene();
		}

	private:
		void onSimulationStarted() override
		{
			Framework::Context::Pop();
		}

		void onSimulationFailed(const Game::ClassicSimulation::WorldFailure reason) override
		{
			Framework::Context::Pop();
		}
	};
}

namespace Scene
{
	class TestClassicClientSimulationSubScene
		:
		public SubBase
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
			return true;
		}

		void onRemove() override
		{
		}

		void onEvent(const sf::Event event) override
		{
		}

		void onLogic(const sf::Time time) override
		{
		}

		void onDraw(sf::RenderTarget* const target) override
		{
			gameView->enable();
			handler->onDraw(target);
		}

	private:
		TestClientSimulationHandler* const handler;
		Device::View* const gameView;
	};

	class TestClassicClientScene
		:
		public Game::Net::ClientClassicConnection,
		public MainSceneBase,
		public Operator::Net::HostFindClassicRequest
	{
	public:
		TestClassicClientScene()
			:
			ClientClassicConnection()
		{
		}
		
		bool onCreate() override
		{
			return Operator::ConnectionHandler::PushRequest(
				Operator::Net::Client::CommonRequestMessageID::HostFindClassic,
				NULL,
				this);
		}

		void onRemove() override
		{
		}

		void initialize() override
		{
		}

		void onShow() override
		{
		}

		void onHide() override
		{
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
		Game::Net::ClassicConnectionInformation connectionInfo;
		Device::View gameView;

		void onRequestFailed(const Reason reason) override
		{
			Log::Error(L"Find host request failed",
				(int) reason, L"reason");

			Framework::Context::Pop();
		}

		void onHostFindClassic(Operator::Net::Host::HostFindClassicMessage* const answer) override
		{
			connectionInfo.address = answer->address;
			connectionInfo.authenticationTimeout = 800; // 20sec
			connectionInfo.key = answer->key;
			connectionInfo.userID = answer->userID;

			if (!ClientClassicConnection::initialize(connectionInfo))
			{
				Log::Error(L"Failed to initialize classic connection");

				Framework::Context::Pop();
			}
		}

		void onHostFindClassicRejected(Operator::Net::Host::HostFindClassicRejectedMessage* const answer) override
		{
			Log::Error(L"Find host request rejected",
				(int) answer->type, L"reason");

			Framework::Context::Pop();
		}
		
		Game::Net::ClientAuthenticationHandler* createAuthenticationHandler(
			ClientAuthenticationHandlerCallback* const callback,
			const sf::Uint32 timeout) override
		{
			return new TestClientAuthenticationHandler(callback, timeout);
		}

		Game::Net::ClientClassicCommonHandler* createCommonHandler() override
		{
			return new TestClientCommonHandler();
		}

		Game::Net::ClientClassicSelectionHandler* createSelectionHandler(
			ClientClassicSelectionHandlerCallback* const callback) override
		{
			return new TestClientSelectionHandler(callback);
		}

		Game::Net::ClientClassicSimulationHandler* createSimulationHandler(
			ClientClassicSimulationHandlerCallback* const callback,
			const Game::SimulationBootInformation info,
			const Game::WorldResourceContainer& worldContainer) override
		{
			TestClientSimulationHandler* const handler = new TestClientSimulationHandler(
				callback,
				info,
				worldContainer,
				&gameView);

			Framework::Context::PushScene(
				new TestClassicClientSimulationSubScene(handler, &gameView),
				false);

			return handler;
		}

		void accessOnRequestFailed(
			const Device::Net::MessageID messageID,
			const ::Net::RequestFailure reason) override
		{
			Framework::Context::Pop();
		}

		void onMessageSendFailed(
			const Device::Net::MessageID messageID,
			const SendFailure reason) override
		{
			Framework::Context::Pop();
		}

		void onInvalidMessageID(const Device::Net::MessageID messageID) override
		{
			Framework::Context::Pop();
		}

		void onConnectionLost(const int reason) override
		{
			// think about allowing lost connections for 
			// about an hour to stay and reconnect if possible

			Framework::Context::Pop();
		}

		void onConnectionClosed(const int reason) override
		{
			Framework::Context::Pop();
		}

		void onProcessFailed() override
		{
			Framework::Context::Pop();
		}
	};
}
