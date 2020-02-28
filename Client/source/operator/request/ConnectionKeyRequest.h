#pragma once

#include <Client/source/operator/request/OperatorRequest.h>
#include <Operator/source/net/CommonRequestMessage.h>

namespace Operator
{
	class ConnectionKeyRequest
		:
		public Request
	{
	public:
		bool processEvent(
			const Device::Net::MessageID messageID,
			void* const rawMessage) override
		{
			switch (messageID)
			{
			/*case Net::Host::RequestMessageID::ConnectionKey:
			{
				Net::Host::ConnectionKeyMessage const* message =
					(Net::Host::ConnectionKeyMessage*) rawMessage;

				onConnectionKeyReceived(message->key);

				return true;
			}
			case Net::Host::RequestMessageID::ConnectionKeyFailed:
				onConnectionKeyFailed();

				return true;
				*/
			}
		}

	protected:
		virtual void onConnectionKeyReceived(const ConnectionKey& key) = 0;
		virtual void onConnectionKeyFailed() = 0;
	};

	class CommonConnectionKeyRequest
		:
		public ConnectionKeyRequest
	{
	public:
		typedef std::function<void(const ConnectionKey&)> ConnectionKeyReceivedCallback;
		typedef std::function<void()> ConnectionKeyFailedCallback;

		CommonConnectionKeyRequest(
			const RequestFailCallback requestFailCallback,
			const ConnectionKeyReceivedCallback connectionKeyReceivedCallback,
			const ConnectionKeyFailedCallback connectionKeyFailedCallback)
			:
			connectionKeyReceivedCallback(connectionKeyReceivedCallback),
			connectionKeyFailedCallback(connectionKeyFailedCallback)
		{
		}

	private:
		const RequestFailCallback requestFailCallback;
		const ConnectionKeyReceivedCallback connectionKeyReceivedCallback;
		const ConnectionKeyFailedCallback connectionKeyFailedCallback;

		void onConnectionKeyReceived(const ConnectionKey& key)
		{
			connectionKeyReceivedCallback(key);
		}

		void onConnectionKeyFailed()
		{
			connectionKeyFailedCallback();
		}

		void onRequestFailed(const Reason reason)
		{
			requestFailCallback(reason);
		}
	};
}
