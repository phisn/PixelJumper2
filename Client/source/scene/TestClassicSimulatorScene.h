#pragma once

#include <Client/source/game/HostClassicSimulator.h>
#include <Client/source/operator/request/CommonRequest.h>
#include <Client/source/scene/MainSceneBase.h>

namespace Scene
{
	class TestClassicSimulatorScene
		:
		public MainSceneBase,
		public Operator::Net::ClassicRegisterHostRequest
	{
	public:
		bool onCreate() override
		{
			return Operator::ConnectionHandler::PushRequest(
				Operator::Net::Client::CommonRequestMessageID::RegisterClassicHost,
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
			if (initialized)
				simulator.onLogic(time);
		}

		void onDraw(sf::RenderTarget* const target) override
		{
		}
		
	private:
		Game::Net::HostClassicSimulator simulator;

		bool initialized = false;

		void onRequestFailed(const Reason reason) override
		{
			Log::Error(L"Register as host request failed",
				(int) reason, L"reason");

			Framework::Context::Pop();
		}

		void onClassicHostRegistratonAccepted() override
		{
			if (!simulator.initialize())
			{
				Log::Error(L"Failed to initialize simulator");
				Framework::Context::Pop();
			}
			else
			{
				initialized = true;
			}
		}

		void onClassicHostRegistratonRejected() override
		{
			Log::Error(L"Registration as host rejected. insufficent priviliges");

			Framework::Context::Pop();
		}
	};
}
