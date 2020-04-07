#pragma once

#include "OperatorAuthScene.h"
#include "TestGameScene.h"
#include "TestOnlineGameScene.h"

namespace Scene
{
	class RootScene
		:
		public Framework::Scene
	{
	public:
		bool onCreate() override
		{
			return true;
		}

		void initialize() override
		{
			if (!Framework::Core::PushScene<OperatorAuthScene>())
			{
				Framework::Core::PopScene();
			}
		}

		void onEvent(const sf::Event event) override
		{
		}

		void onLogic(const sf::Time time) override
		{
			ImGui::BeginMainMenuBar();

			if (ImGui::MenuItem("Settings"))
			{
				Framework::Core::PushScene<SettingsScene>();
			}

			ImGui::EndMainMenuBar();
			
			Framework::ImGuiSetRelativeNextWindowPos(ImVec2{ 0.5f, 0.5f });
			ImGui::Begin("Selection", NULL, Framework::WindowFlagsStatic);

			if (ImGui::Button("Run", ImVec2{ 200, 0 }))
				Framework::Core::PushScene<TestClassicClientScene>();

			if (ImGui::Button("Close", ImVec2{ 200, 0 }))
				Framework::Core::PopScene();

			ImGui::End();
		}

		void onDraw(sf::RenderTarget* const target) override
		{
		}

		void onShow() override
		{
			if (Operator::Client::GetAuthenticationStatus() != Operator::Client::AuthenticationStatus::Authenticated)
			{
				Framework::Core::PopScene();
			}
		}

		void onHide() override
		{
		}

	private:
		bool authenticated = false;
	};
}
