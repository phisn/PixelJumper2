#pragma once

#include <Client/source/device/NetDevice.h>
#include <Client/source/game/net/NetworkMessage.h>

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
			const Device::Net::MessageID messageID,
			const RequestFailure reason) = 0;
		// for messages without message content null can be passed as
		// message
		// result usually not used
		virtual bool accessSendMessage(
			const Device::Net::MessageID messageID,
			Game::Net::NetworkMessage* const message = NULL) = 0;
		virtual void accessOnThreatIdentified(
			const Device::Net::MessageID messageID,
			const wchar_t* const note,
			const Device::Net::ThreatLevel level) = 0;
	};

}