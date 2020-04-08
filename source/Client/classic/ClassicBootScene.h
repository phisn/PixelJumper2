#pragma once

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/imgui/LoadingModalWindow.h"
#include "FrameworkCore/scene/InformationScene.h"

#include "OperatorClient/OperatorClient.h"
#include "OperatorClient/request/FindClassicHostRequest.h"

namespace Scene
{
	class ClassicBootScene
		:
		public Operator::FindClassicHostRequest,
		public Framework::Scene
	{
		enum class State
		{
			Boot,
			Running,
			Stop

		} state = State::Boot;

	public:
		void initialize() override
		{
			Operator::Client::PushRequestFailure result = Operator::Client::PushRequest(
				Net::Client::OperatorCommonMessageID::HostFindClassic,
				NULL,
				this);

			if (result != Operator::Client::PushRequestFailure::Success)
			{
				Log::Error(L"classic boot failed",
					(int) result, L"reason");

				Framework::Core::PushTemporaryScene<::Scene::InformationScene>(
					"Failed to find host (reason: " 
					+ std::to_string((int) result)
					+ ")");
			}

			window.open();
		}

		void onEvent(const sf::Event event) override
		{
		}

		void onLogic(const sf::Time time) override
		{
			ImGui::Begin("placeholder", NULL, Framework::WindowFlagsOverlay);
			// can not be closed
			window.process();
			ImGui::End();
		}

		void onDraw(sf::RenderTarget* const target) override
		{
		}

	private:
		Framework::LoadingModalWindow window;

		void onClassicHostFound(Net::Host::HostFindClassicMessage& message) override
		{
		}

		void onClassicHostFindRejected(Net::Host::HostFindClassicRejectedMessage& message) override
		{
			Log::Error(L"Findhost request rejected",
				(int) message.type, L"reason");

			switch (message.type)
			{
			case Net::Host::HostFindClassicRejectedMessage::InvalidUserMode:
				Framework::Core::PushTemporaryScene<::Scene::InformationScene>(
					"Failed to find classic host, invalid user mode");

				break;
			case Net::Host::HostFindClassicRejectedMessage::NoHostAvailable:
				Framework::Core::PushTemporaryScene<::Scene::InformationScene>(
					"Currently no classic host available");

				break;
			}

		}

		void onRequestFailure(const RequestInterface::Reason reason) override
		{
			Log::Error(L"Findhost request failed",
				(int) reason, L"reason");

			Framework::Core::PushTemporaryScene<::Scene::InformationScene>(
				"Failed to find host (reason: "
				+ std::to_string((int) reason)
				+ ")");
		}
	};
}
