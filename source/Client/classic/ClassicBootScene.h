#pragma once

#include "ClassicConnectionScene.h"

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
			findClassicHost();
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

		void onShow() override
		{
			// if classic connection scene pops child scene
			// restart connection
			findClassicHost();
		}

	private:
		Framework::LoadingModalWindow window;

		void findClassicHost()
		{
			Operator::Client::PushRequestFailure result = Operator::Client::PushRequest(
				Net::Client::OperatorCommonMessageID::HostFindClassic,
				NULL,
				this);

			if (result != Operator::Client::PushRequestFailure::Success)
			{
				Log::Error(L"classic boot failed",
					(int)result, L"reason");

				Framework::Core::PushChildScene<::Scene::InformationScene>(
					"Failed to find host (reason: "
					+ std::to_string((int)result)
					+ ")");
			}
		}

		void onClassicHostFound(Net::Host::HostFindClassicMessage& message) override
		{
			ClassicConnectionScene* scene = new ClassicConnectionScene;

			Game::ClassicConnectionInformation info;
			info.address = message.content.address;
			info.key = message.content.key;

			if (scene->initialize(info))
			{
				Framework::Core::PushChildScene(scene);
			}
			else
			{
				delete scene;

				Framework::Core::PushChildScene<::Scene::InformationScene>(
					"Failed to open connection to classic host");
			}
		}

		void onClassicHostFindRejected(Net::Host::HostFindClassicRejectedMessage& message) override
		{
			Log::Error(L"Findhost request rejected",
				(int) message.content.type, L"reason");

			switch (message.content.type)
			{
				/*
			case Net::Host::HostFindClassicRejectedMessageContent::InvalidUserMode:
				Framework::Core::PushChildScene<::Scene::InformationScene>(
					"Failed to find classic host, invalid user mode");

				break;
				*/
			case Net::Host::HostFindClassicRejectedMessageContent::NoHostAvailable:
				Framework::Core::PushChildScene<::Scene::InformationScene>(
					"Currently no classic host available");

				break;
			}

		}

		void onRequestFailure(const RequestInterface::Reason reason) override
		{
			Log::Error(L"Findhost request failed",
				(int) reason, L"reason");

			Framework::Core::PushChildScene<::Scene::InformationScene>(
				"Failed to find host (reason: "
				+ std::to_string((int) reason)
				+ ")");
		}
	};
}
