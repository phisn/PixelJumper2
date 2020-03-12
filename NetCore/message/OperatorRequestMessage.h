#pragma once

#include "NetCore/NetCore.h"
#include "NetCore/NetworkMessage.h"

namespace Net
{
	typedef sf::Uint64 RequestID;

	namespace Client
	{
		struct OperatorRequestMessageID
		{
			enum
			{
				_Begin = ::Net::CommonMessageID::_Offset - 1,

				OperatorRequest,

				_Offset
			};
		};
	}

	namespace Host
	{
		struct OperatorRequestMessageID
		{
			enum
			{
				_Begin = ::Net::CommonMessageID::_Offset - 1,

				OperatorRequest,

				_Offset
			};
		};
	}

	class OperatorRequestMessage
		:
		public NetworkMessage
	{
	public:
		struct Content
		{
			RequestID requestID;
			MessageID messageID;

		} content;

		// payload only used for save
		NetworkMessage* payload;

		bool load(Resource::ReadPipe* const pipe) override
		{
			return pipe->readValue(&content);
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			if (!pipe->writeValue(&content))
			{
				return false;
			}

			if (payload)
			{
				return payload->save(pipe);
			}

			return true;
		}
	};
}