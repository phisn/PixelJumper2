#pragma once

#include "game/ClientClassicConnection.h"

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/imgui/LoadingModalWindow.h"
#include "FrameworkCore/scene/InformationScene.h"
#include "FrameworkCore/scene/QuestionScene.h"

namespace Scene
{
	class ClassicConnectionScene
		:
		public Game::ClientClassicConnection,
		public Framework::Scene
	{
	public:
		ClassicConnectionScene()
		{
		}

		void initialize() override
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
		}

		using ClientClassicConnection::initialize;

	private:
		void accessOnRequestFailed(
			const Net::MessageID messageID,
			const Net::RequestFailure reason) override
		{

		}

		void onMessageSendFailed(
			const Net::MessageID messageID, 
			const Net::SendFailure failure) override
		{

		}

		void onConnectionLost(const int reason) override
		{
			std::string question_reason;

			Framework::Core::PushScene<::Scene::QuestionScene>(
				[](bool accepted)
				{
					if (accepted)
					{
						Framework::Core::PopChildScene();
					}
					else
					{
						Framework::Core::PopScene();
					}

				}, question_reason + ", do you want to reconnect?");
		}

		void onConnectionClosed(const int reason) override
		{

		}

		Game::ClientAuthenticationHandler* createAuthenticationHandler(const Game::ClientAuthenticationHandlerArguments& arguments) override
		{
			return NULL;
		}

		Game::ClientClassicSelectionHandler* createSelectionHandler() override
		{
			return NULL;
		}

		Game::ClientClassicSessionHandler* createSessionHandler(const Game::ClientClassicSessionHandlerArguments& arguments) override
		{
			return NULL;
		}

		Game::ClientClassicSimulationHandler* createSimulationHandler(const Game::ClientClassicSimulationHandlerArguments& arguments) override
		{
			return NULL;
		}
	};
}
