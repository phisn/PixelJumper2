#pragma once


#include <Client/source/net/DynamicClientHandler.h>

#include <Operator/source/operator/AuthenticationHandler.h>
#include <Operator/source/operator/ClassicHostRequestHandler.h>
#include <Operator/source/operator/CommonRequestHandler.h>

namespace Operator::Net
{
	typedef sf::Uint32 Age;

	class OperatorClientHandler
		:
		public ::Net::DynamicClientHandler,
		public AuthenticationHandlerCallback,
		public CommonRequestHandlerCallback
	{
	public:
		enum class Status
		{
			Authenticating,
			Authenticated,

			// user is officially hosting and
			// should not be disconnected
			Hosting,

			// cause for closing is not specified
			// it can be an error, timeout, rejection
			// or different. if bad that it is already
			// reported as threat
			Closing
		};

		OperatorClientHandler(
			const HSteamNetConnection connection,
			const sf::Uint32 timeout = 100)
			:
			DynamicClientHandler(connection)
		{
			addRequestHandler(
				new AuthenticationHandler(
					this,
					timeout)
			);
		}

		virtual void update()
		{
			if (!process())
			{
				status = Status::Closing;

				// think about removing this because
				// it has a very high chance to fail
				// (gurranted?)
				beginMessage(Game::Net::CommonMessageID::InternalError, 8);
				sendMessage();

				return;
			}

			++age;

			callHandlersUpdate();
		}

		Status getStatus() const
		{
			return status;
		}

		bool isAuthenticated() const
		{
			return status == Status::Authenticated;
		}

		Age getAge() const
		{
			return age;
		}

	private:
		using ClientHandler::process;

		UserID userID = NULL;
		Status status = Status::Authenticating;
		Age age = 0;

		void onAuthenticated(const UserID userID) override
		{
			Log::Information(L"Client authenticated",
				userID, L"userID");

			status = Status::Authenticated;
			this->userID = userID;

			delete removeRequestHandler<AuthenticationHandler>();
			addRequestHandler<CommonRequestHandler>(
				new CommonRequestHandler(
					this,
					userID)
			);
		}

		void onAuthenticationDenied() override
		{
			status = Status::Closing;
		}

		void registerAsClassicHost() override
		{
			if (status != Status::Hosting)
			{
				status = Status::Hosting;

				addRequestHandler<ClassicHostRequestHandler>(
					new ClassicHostRequestHandler(
						userID)
				);
			}
		}

		void onThreatIdentified(
			const Device::Net::MessageID messageID,
			const wchar_t* const note,
			const Device::Net::ThreatLevel level) override
		{
			Log::Warning(L"Threat identified (" + std::wstring(note) + L")",
				messageID, L"messageID",
				(int)level, L"level");
		}


		void onInvalidMessageID(const Device::Net::MessageID messageID) override
		{
			accessSendMessage(
				Game::Net::CommonMessageID::InternalError,
				NULL);
			status = Status::Closing;
		}

		void onMessageSendFailed(
			const Device::Net::MessageID messageID,
			const SendFailure reason) override
		{
			accessSendMessage(
				Game::Net::CommonMessageID::InternalError,
				NULL);
			status = Status::Closing;
		}

		void accessOnRequestFailed(
			const Device::Net::MessageID messageID,
			const ::Net::RequestFailure reason) override
		{
			accessSendMessage(
				Game::Net::CommonMessageID::InternalError,
				NULL);
			status = Status::Closing;
		}
	} ;
}
