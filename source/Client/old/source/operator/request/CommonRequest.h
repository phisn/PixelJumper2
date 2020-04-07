#pragma once

#include <Client/source/operator/request/OperatorRequest.h>
#include <Operator/source/net/CommonRequestMessage.h>

namespace Operator::Net
{
	class HostFindClassicRequest
		:
		public Request
	{
	public:
		bool processEvent(
			const Device::Net::MessageID messageID,
			void* const message) override
		{
			switch (messageID)
			{
			case Operator::Net::Host::CommonRequestMessageID::HostFindClassic:
				onHostFindClassic(
					(Operator::Net::Host::HostFindClassicMessage*) message
				);

				return true;
			case Operator::Net::Host::CommonRequestMessageID::HostFindClassicRejected:
				onHostFindClassicRejected(
					(Operator::Net::Host::HostFindClassicRejectedMessage*) message
				);

				return true;
			}

			return false;
		}

	protected:
		virtual void onHostFindClassic(Operator::Net::Host::HostFindClassicMessage* const answer) = 0;
		virtual void onHostFindClassicRejected(Operator::Net::Host::HostFindClassicRejectedMessage* const answer) = 0;
	};

	class ClassicRegisterHostRequest
		:
		public Request
	{
	public:
		bool processEvent(
			const Device::Net::MessageID messageID,
			void* const message) override
		{
			switch (messageID)
			{
			case Operator::Net::Host::CommonRequestMessageID::RegisterClassicHostAccepted:
				onClassicHostRegistratonAccepted();

				return true;
			case Operator::Net::Host::CommonRequestMessageID::RegisterClassicHostRejected:
				onClassicHostRegistratonRejected();

				return true;
			}

			return false;
		}

	protected:
		virtual void onClassicHostRegistratonAccepted() = 0;
		virtual void onClassicHostRegistratonRejected() = 0;
	};
}
