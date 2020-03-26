#pragma once

#include "FrameworkCore/FrameworkCore.h"
#include "OperatorClient/OperatorClient.h"
#include "OperatorClient/request/AuthenticationRequest.h"
#include "imgui/imgui.h"

// maybe move to framework core 
// later to reuse in simulator
namespace Scene
{
	class OperatorAuthScene
		:
		public Framework::Scene,
		public ::Operator::ClientAuthenticationRequest
	{
	public:
		bool onCreate() override
		{
			return true;
		}

		void initialize() override
		{
		}

		void onEvent(const sf::Event event) override
		{
		}

		void onLogic(const sf::Time time) override
		{
			ImGui::SetNextWindowPosCenter();
			ImGui::Begin("Login", NULL, 
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::SetWindowFontScale(2.f);

			show_login();

			ImGui::End();
		}

		void onDraw(sf::RenderTarget* const target) override
		{
		}

		void onShow() override
		{
		}

		void onHide() override
		{
		}

	private:
		void onAuthenticationFailed(const Reason reason) override
		{
		}

		void onRegistrationFailed(const ::Net::Host::RejectRegistrationMessage::Reason reason) override
		{

		}

		void onAuthenticated(const Operator::UserID userID) override
		{
		}

		void onAuthenticated(
			const char token[OPERATOR_HASH_SIZE],
			const Operator::UserID userID) override
		{
		}

		void show_login()
		{
			ImGui::Text("Username");
			ImGui::SameLine();
			ImGui::InputText("###username", username, 26);
			ImGui::NextColumn();
			ImGui::Text("Password");
			ImGui::SameLine();
			ImGui::InputText("###password", password, 26, ImGuiInputTextFlags_Password);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			static Operator::Client::PushRequestFailure result;
			if (ImGui::Button("Login"))
			{
				::Net::Client::AuthenticationMessage message;
				result = Operator::Client::PushRequest(
					::Net::Client::OperatorAuthenticationMessageID::Authenticate,
					&message,
					this);

				if (result != Operator::Client::PushRequestFailure::Success)
				{
					ImGui::OpenPopup("pushrequest_failure");
					ImGui::Text("Push request failed (code: %d)", (int) result);
				}

				memset(password, 0, 26);
			}

			if (ImGui::BeginPopupModal("pushrequest_failure", NULL, 
					ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoMove |
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_AlwaysAutoResize))
			{
				if (ImGui::IsMouseClicked(0))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::Text("Push request failed (code: %d)", (int)result);
				ImGui::EndPopup();
			}
		}

		char username[26];
		char password[26];
	};
}
