#pragma once

#include "SettingsScene.h"

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/ImGuiUtil.h"
#include "FrameworkCore/ImGuiWindowComponent.h"
#include "OperatorClient/OperatorClient.h"
#include "OperatorClient/request/AuthenticationRequest.h"

// maybe move to framework core 
// later to reuse in simulator
namespace Scene
{
	constexpr size_t password_limit = 26;
	constexpr size_t username_limit = 26;
	constexpr size_t key_size = 18;

	class RootWindow
		:
		public Framework::ImGuiWindowComponent
	{
	public:
		RootWindow()
			:
			ImGuiWindowComponent(
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoBackground |
				ImGuiWindowFlags_MenuBar)
		{
			title = "###root_window";
			useActive = true;
		}

		bool begin() override
		{
			ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
			ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

			if (ImGuiWindowComponent::begin())
			{

				return true;
			}

			return false;
		}

		void end() override
		{
			ImGuiWindowComponent::end();
		}
	};

	class LoginWindow
		:
		public Framework::ImGuiWindowComponent
	{
	public:
		LoginWindow()
			:
			ImGuiWindowComponent(WindowFlagsStatic)
		{
			title = "login_window";
			Framework::ImGuiApplyColorStyleLight();
		}

		bool begin() override
		{
			Framework::ImGuiSetRelativeNextWindowPos(ImVec2(0.5f, 0.5f));

			if (ImGuiWindowComponent::begin())
			{
				return true;
			}

			return false;
		}

		void end() override
		{
			ImGuiWindowComponent::end();
		}
	};

	class QuestionWindow
	{
	public:
		typedef std::function<void(bool)> Callback;

		void process()
		{
			if (active)
			{
				ImGui::OpenPopup(name);
				Framework::ImGuiSetRelativeNextWindowPos(ImVec2(0.5f, 0.5f));
			}

			if (ImGui::BeginPopupModal(name, NULL, Framework::ImGuiWindowComponent::WindowFlagsStatic))
			{
				ImGui::Text(question.c_str());
				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				if (ImGui::Button("Yes"))
				{
					ImGui::CloseCurrentPopup();
					active = false;
					callback(true);
				}
				ImGui::SameLine();

				if (ImGui::Button("No"))
				{
					ImGui::CloseCurrentPopup();
					active = false;
					callback(false);
				}

				ImGui::EndPopup();
			}
		}
		
		void open(const Callback callback, const std::string question)
		{
			this->callback = callback;
			this->question = question;

			active = true;
		}

		bool isActive() const
		{
			return active;
		}

	private:
		const char* name = "###question_window";
		bool active;

		Callback callback;
		std::string question;
	};
	
	class LoadingWindow
	{
	public:
		void process()
		{
			if (active)
			{
				ImGui::OpenPopup(name);
			}

			ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, sf::Color::Color(0, 0, 0, 200));

			ImGuiIO& io = ImGui::GetIO();
			ImGui::SetNextWindowPos(
				ImVec2(io.DisplaySize.x * 0.99f, io.DisplaySize.y * 0.99f),
				0,
				ImVec2{ 1.f, 1.f });
			if (ImGui::BeginPopupModal(name, NULL, Framework::ImGuiWindowComponent::WindowFlagsOverlay))
			{
				const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
				const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);

				Framework::LoadingSpinner("##spinner", 30, sf::Color::Color(230, 230, 230), sf::Color::Color(150, 150, 150), 8, 4.f);

				ImGui::SetWindowSize(
					(sf::Vector2f) ImGui::GetWindowContentRegionMin() + sf::Vector2f{ 20, 20 }
				);

				if (!active)
					ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}

			ImGui::PopStyleColor();
		}

		void open()
		{
			active = true;
		}

		void close()
		{
			active = false;
		}

		bool isActive() const
		{
			return active;
		}

	private:
		const char* name = "loading_window";
		bool active = false;
	};

	class CenteredInformationWindow
	{
	public:
		void process()
		{
			if (active)
			{
				ImGui::OpenPopup(name);
				Framework::ImGuiSetRelativeNextWindowPos(ImVec2(0.5f, 0.5f));
			}

			if (ImGui::BeginPopupModal(name, NULL, Framework::ImGuiWindowComponent::WindowFlagsStatic))
			{
				ImGui::Text(text.c_str());
				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				if (ImGui::Button("Close"))
				{
					ImGui::CloseCurrentPopup();
					active = false;
				}

				ImGui::EndPopup();
			}
		}

		void open(const std::string text)
		{
			this->text = text;
			active = true;
		}

		bool isActive() const
		{
			return active;
		}

	private:
		const char* name = "centered_information_window";

		std::string text;
		bool active = false;
	};

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
			if (!window.begin())
			{
				window.end();
				return;
			}

			ImGui::BeginMainMenuBar();

			if (ImGui::MenuItem("Settings"))
			{
				Framework::Core::PushScene<SettingsScene>();
			}
			
			if (Framework::ImGuiTRCloseButton())
			{
				questionWindow.open(
					[](const bool close)
					{
						if (close)
						{
							Framework::Core::PopScene();
						}
					}, "Do you really want do leave?");
			}

			ImGui::EndMainMenuBar();

			ImGui::InputTextEx(
				"###username", "Username",
				username, username_limit, ImVec2(0, 0),
				0);
			ImGui::NextColumn();
			ImGui::InputTextEx(
				"###password", "Password",
				password, password_limit, ImVec2(0, 0),
				ImGuiInputTextFlags_Password);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::Button("Login") && validateUsernamePassword())
			{
				Net::Client::AuthenticationMessage* message = new Net::Client::AuthenticationMessage();

				Module::Encryption::HashCommon(
					(unsigned char*) message->content.hash,
					(unsigned char*) password,
					strlen(password));

				message->username = (const char*) username;

				Operator::Client::PushRequestFailure result = Operator::Client::PushRequest(
					::Net::Client::OperatorAuthenticationMessageID::Authenticate,
					message,
					this);

				if (result != Operator::Client::PushRequestFailure::Success)
				{
					informationWindow.open("Authentication failed (reason: " + std::to_string((int) result) + ")");
				}
				else
				{
					loadingWindow.open();
				}

				memset(password, 0, password_limit);
			}

			ImGui::SameLine();
			if (ImGui::Button("Register") && validateUsernamePassword())
			{
				ImGui::OpenPopup("register_popup");
			}

			informationWindow.process();
			loadingWindow.process();
			questionWindow.process();

			if (ImGui::BeginPopupModal("register_popup", NULL,
					Framework::ImGuiWindowComponent::WindowFlagsStatic))
			{
				ImGui::InputTextEx(
					"###repeat_password", "Repeat password", 
					repeat_password, password_limit, ImVec2(0, 0), 
					ImGuiInputTextFlags_Password);
				Framework::InputTextKey("###key", "Key", key, key_size, ImVec2(0, 0));

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();
				
				if (ImGui::Button("Register###2"))
				{
					if (memcmp(password, repeat_password, password_limit) == 0)
					{
						if (strlen(key) == key_size - 1)
						{
							Net::Client::RegistrationMessage* message = new Net::Client::RegistrationMessage();

							Module::Encryption::HashCommon(
								(unsigned char*) message->content.hash,
								(unsigned char*) password,
								strlen(password));

							message->username = (const char*) username;
							for (int i = 0; i < key_size; ++i)
								if ((i + 1) % 6 != 0)
								{
									message->content.key.content[i - (i + 1) / 6] = key[i];
								}

							Operator::Client::PushRequestFailure result = Operator::Client::PushRequest(
								::Net::Client::OperatorAuthenticationMessageID::Register,
								message,
								this);

							if (result == Operator::Client::PushRequestFailure::Success)
							{
								loadingWindow.open();
							}
							else
							{
								informationWindow.open("Registration failed (reason: " + std::to_string((int)result) + ")");
							}
						}
						else
						{
							informationWindow.open("Invalid key size");
						}
					}
					else
					{
						informationWindow.open("Repeated password was wrong");
					}

					memset(password, 0, password_limit);
					memset(repeat_password, 0, password_limit);

					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
					memset(repeat_password, 0, password_limit);
				}

				ImGui::EndPopup();
			}
		
			window.end();
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
		LoadingWindow loadingWindow;
		CenteredInformationWindow informationWindow;
		LoginWindow window;
		QuestionWindow questionWindow;
		RootWindow rootWindow;

		char username[username_limit];
		char password[password_limit];

		char repeat_password[password_limit];
		char key[17];

		bool validateUsernamePassword()
		{
			if (username[0] == '\0' ||
				password[0] == '\0')
			{
				informationWindow.open("Neither username nor password can be empty");
				return false;
			}

			return true;
		}

		void onRequestFailure(const RequestInterface::Reason reason) override
		{
			ClientAuthenticationRequest::onRequestFailure(reason);

			loadingWindow.close();

			switch (reason)
			{
				// here normally equals to connection failed
			case RequestInterface::Reason::ConnectionLost:
			case RequestInterface::Reason::ConnectFailed:
				informationWindow.open("Failed to connect to operator");
				
				break;
			case RequestInterface::Reason::InternalError:
				informationWindow.open("Operator internal error");

				break;
			case RequestInterface::Reason::TimeoutUnconnected:
				informationWindow.open("Request timed out, failed to connect to operator");

				break;
			case RequestInterface::Reason::TimeoutUnresponeded:
				informationWindow.open("Operator did not reply to request");

				break;
			default:
				informationWindow.open("Request failed (reason: " + std::to_string((int)reason) + ")");

				break;
			}
		}

		void onAuthenticationFailed(const Reason reason) override
		{
			loadingWindow.close();

			switch (reason)
			{
			case Reason::Rejected:
				informationWindow.open("Authentication rejected");

				break;
			case Reason::Timeout:

				informationWindow.open("Authentication external timeout");
			}

		}

		void onRegistrationFailed(const ::Net::Host::RejectRegistrationMessage::Reason reason) override
		{
			loadingWindow.close();

			switch (reason)
			{
			case Net::Host::RejectRegistrationMessage::Reason::InternalError:
				informationWindow.open("Registration failed, internal error");

				break;
			case Net::Host::RejectRegistrationMessage::Reason::KeyInvalid:
				informationWindow.open("Registration failed, invalid key");

				break;
			case Net::Host::RejectRegistrationMessage::Reason::KeyUsed:
				informationWindow.open("Registration failed, key already used");

				break;
			case Net::Host::RejectRegistrationMessage::Reason::UsernameUsed:
				informationWindow.open("Registration failed, username already used");

				break;
			}
		}

		void onAuthenticated(const Operator::UserID userID) override
		{
			loadingWindow.close();
			Framework::Core::PopScene();
		}

		void onAuthenticated(
			const char token[OPERATOR_HASH_SIZE],
			const Operator::UserID userID) override
		{
			loadingWindow.close();
			Framework::Core::PopScene();
		}

	};
}
