#pragma once

#include "NetCore.h"
#include "NetworkMessage.h"

namespace Net
{
	enum class RequestFailure
	{
		Loading,
		Internal,
		External
	};

	// all functions are abstract because of the way they
	// will be implemented. a connection from the client to the
	// operator does not want to close the connection when
	// a message can not be send. the operator will always want
	// to close the connection on every error
	struct ConnectionAccess
	{
		// request guranteed to return immediately and can
		// be directly shutdown
		virtual void accessOnRequestFailed(
			const MessageID messageID,
			const RequestFailure reason) = 0;
		// for messages without message content null can be passed as
		// message
		// result usually not used
		virtual bool sendMessage(
			const MessageID messageID,
			NetworkMessage* const message = NULL,
			const int flags = k_nSteamNetworkingSend_Reliable) = 0;
		virtual void onThreatIdentified(
			const sf::Uint32 identifier,
			const std::wstring& note,
			const ThreatLevel level) = 0;
	};

	class RequestHandler
	{
	public:
		virtual ~RequestHandler()
		{
		}

		virtual void initialize(ConnectionAccess* const access)
		{
			this->access = access;
		}

		virtual void update() = 0;

		// true when messageid was used to find messages with
		// unused (supsicous) ids
		virtual bool onMessage(
			const MessageID messageID,
			Resource::ReadPipe* const pipe) = 0;

	protected:
		// request is needed to return immediately and
		// not acceess internal values when reveivced false
		bool loadMessage(
			const MessageID messageID,
			NetworkMessage* const message,
			Resource::ReadPipe* const pipe)
		{
			if (!message->load(pipe))
			{
				access->onThreatIdentified(
					messageID,
					L"failed to load message",
					ThreatLevel::Suspicious);
				access->accessOnRequestFailed(
					messageID,
					RequestFailure::Loading);

				return false;
			}

			return true;
		}

		ConnectionAccess* access;
	};
}
