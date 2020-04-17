#pragma once

#include "RequestInterface.h"

#include "NetCore/message/OperatorClassicSimulatorMessage.h"

namespace Operator
{
	class ClassicHostClientDataRequest
		:
		public RequestInterface
	{
	public:
		bool request_onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Net::Host::OperatorClassicHostID::ClientDataReply:
				if (Net::Host::OperatorClassicHost::ClientDataMessage message; request_loadMessage(messageID, &message, pipe))
				{
					onClassicClientData(message);
				}

				return true;
			case Net::Host::OperatorClassicHostID::ClientDataRequestFailed:
				if (Net::Host::OperatorClassicHost::ClientDataRequestFailedMessage message; request_loadMessage(messageID, &message, pipe))
				{
					onRequestClassicClientDataFailed(message);
				}

				return true;
			}

			return false;
		}

	protected:
		virtual void onClassicClientData(Net::Host::OperatorClassicHost::ClientDataMessage& message) = 0;
		virtual void onRequestClassicClientDataFailed(Net::Host::OperatorClassicHost::ClientDataRequestFailedMessage& message) = 0;
	};

	class ClassicHostRegisterClientRequest
		:
		public RequestInterface
	{
	public:
		bool request_onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Net::Host::OperatorClassicHostID::ClientDataReply:
				if (Net::Host::OperatorClassicHost::ClientRegisteredMessage message; request_loadMessage(messageID, &message, pipe))
				{
					onClientRegistered(message);
				}

				return true;
			case Net::Host::OperatorClassicHostID::ClientDataRequestFailed:
				if (Net::Host::OperatorClassicHost::ClientRegistrationFailedMessage message; request_loadMessage(messageID, &message, pipe))
				{
					onClientRegisterFailed(message);
				}

				return true;
			}

			return false;
		}

	protected:
		virtual void onClientRegistered(Net::Host::OperatorClassicHost::ClientRegisteredMessage &message) = 0;
		virtual void onClientRegisterFailed(Net::Host::OperatorClassicHost::ClientRegistrationFailedMessage& message) = 0;
	};

	class ClassicHostUnregisterClientRequest
		:
		public RequestInterface
	{
	public:
		bool request_onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Net::Host::OperatorClassicHostID::ClientUnregistered:
				onClientUnregistered();

				return true;
			}

			return false;
		}

	protected:
		virtual void onClientUnregistered() = 0;
	};

	class WorldUnlockRequest
		:
		public RequestInterface
	{
	public:
		bool request_onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Net::Host::OperatorClassicHostID::UnlockedWorld:
				onWorldUnlocked();

				return true;
			case Net::Host::OperatorClassicHostID::UnlockWorldFailed:
				onWorldUnlockFailed();

				return true;
			}

			return false;
		}

	protected:
		virtual void onWorldUnlocked() = 0;
		virtual void onWorldUnlockFailed() = 0;
	};

	class RepresentationUnlockRequest
		:
		public RequestInterface
	{
	public:
		bool request_onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Net::Host::OperatorClassicHostID::UnlockedRepresentation:
				onRepresentationUnlocked();

				return true;
			case Net::Host::OperatorClassicHostID::UnlockRepresentationFailed:
				onRepresentationUnlockFailed();

				return true;
			}

			return false;
		}

	protected:
		virtual void onRepresentationUnlocked() = 0;
		virtual void onRepresentationUnlockFailed() = 0;
	};
}
