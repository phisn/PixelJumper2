#pragma once

#include <Client/source/operator/AuthenticationRequest.h>
#include <Client/source/operator/OperatorConnectionHandler.h>

#include <Client/source/scene/MainSceneBase.h>

/*

register test:
{
	SteamNetworkingIPAddr ipAddress;
	ipAddress.SetIPv6LocalHost(9928);

	Operator::ConnectionHandler::Initialize(ipAddress);

	const std::string username = "user1";

	char hash[32] =
	{
		0x28, 0xa1, 0xad, 0xb3, 0x99,
		0xac, 0x19, 0x0b, 0x8e, 0xe9,
		0xbb, 0x14, 0xd4, 0x4a, 0xa5,
		0xa2, 0x86, 0x25, 0x51, 0x3f,
		0x95, 0x12, 0x03, 0x29, 0x18,
		0x00, 0x23, 0x82, 0x91, 0x85,
		0x48, 0x12
	};

	const char* key = "TSNEDMNGZTSYEOT";

	Operator::Net::Client::RegistrationMessage* message =
		new Operator::Net::Client::RegistrationMessage();

	memcpy(&message->content.hash,
		hash,
		OPERATOR_HASH_SIZE);

	memcpy(&message->content.key,
		key,
		OPERATOR_KEY_SIZE);

	message->username = username;

	Operator::CommonRegistrationRequest* request = new Operator::CommonRegistrationRequest(
		[request, message](const Operator::UserID user)
		{
			Log::Information(L"accepted", user, L"user");
		},
		[request, message](const Operator::Net::Host::RejectRegistrationMessage::Reason reason)
		{
			Log::Information(L"rejected", (int) reason, L"reason");
		},
		[request, message](const Operator::Request::Reason reason)
		{
			Log::Information(L"failed", (int) reason, L"reason");
		});

	if (!Operator::ConnectionHandler::PushRequest(
			Operator::Net::Client::AuthMessageID::Register,
			message,
			request))
	{
		Log::Information(L"failed to push request");
	}

	return true;
}

*/

namespace Scene
{
	class AuthenticationScene
		:
		public MainSceneBase
	{
	public:
		bool onCreate() override
		{
			SteamNetworkingIPAddr ipAddress;
			ipAddress.SetIPv6LocalHost(9928);

			Operator::ConnectionHandler::Initialize(ipAddress);			

			const std::string username = "user1";

			char hash[32] = 
			{ 
				0x28, 0xa1, 0xad, 0xb3, 0x99, 
				0xac, 0x19, 0x0b, 0x8e, 0xe9, 
				0xbb, 0x14, 0xd4, 0x4a, 0xa5, 
				0xa2, 0x86, 0x25, 0x51, 0x3f,
				0x95, 0x12, 0x03, 0x29, 0x18,
				0x00, 0x23, 0x82, 0x91, 0x85,
				0x48, 0x12
			};

			Operator::Net::Client::AuthenticationMessage* message =
				new Operator::Net::Client::AuthenticationMessage();

			memcpy(&message->content.hash,
				hash,
				OPERATOR_HASH_SIZE);

			message->username = username;

			Operator::AuthenticationRequest* request = new Operator::CommonAuthenticationRequest(
				[request, message](const Operator::UserID user)
				{
					Log::Information(L"accepted", user, L"user");
				},
				[request, message]()
				{
					Log::Information(L"rejected");
				},
				[request, message](const Operator::Request::Reason reason)
				{
					Log::Information(L"failed", (int) reason, L"reason");
				});

			if (!Operator::ConnectionHandler::PushRequest(
					Operator::Net::Client::AuthMessageID::Authenticate,
					message,
					request))
			{
				Log::Information(L"failed to push request");
			}
			
			return true;
		}

		void onScenePopped(const int size) override
		{
		}

		void initialize() override
		{
		}

		void onLogic(const sf::Time time) override
		{
			Operator::ConnectionHandler::Process(time);
		}

		void onDraw(sf::RenderTarget* const target) override
		{
		}

		void onRemove() override { }
		void onShow() override { }
		void onHide() override { }
		void onEvent(const sf::Event event) override { }
	};
}
