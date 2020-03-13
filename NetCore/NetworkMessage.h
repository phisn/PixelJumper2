#pragma once

#include "Resource/pipes/PipeBase.h"

namespace Net
{
	struct NetworkMessage
	{
		virtual ~NetworkMessage()
		{
		}

		virtual bool load(Resource::ReadPipe* const pipe) = 0;
		virtual bool save(Resource::WritePipe* const pipe) = 0;
	};

	template <typename Content>
	struct TrivialNetworkMessage
		:
		public NetworkMessage,
		public Content
	{
		bool load(Resource::ReadPipe* const pipe) override
		{
			return pipe->readValue(this);
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return pipe->writeValue(this);
		}
	};

	struct CommonMessageID
	{
		enum
		{
			// only internally important and no
			// message or code is provided to the
			// user
			InternalError,

			// external error often caused by the user
			// with error message and error code
			ExternalError,

			_Offset
		};
	};

	class ExternalErrorMessage
		:
		public NetworkMessage
	{
	public:
		// message should not be longer
		// than 255 characters
		std::wstring message;

		struct Content
		{
			sf::Uint64 errorID;

			// cause messageID when error
			// is send in response to a message
			sf::Uint64 messageID;

		} content;

		bool load(Resource::ReadPipe* const pipe) override
		{
			return pipe->readValue(&content)
				&& pipe->readString(&message);
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return pipe->writeValue(&content)
				&& pipe->writeString(&message);
		}
	};

	struct CommonErrorID
	{
		enum
		{
			InvalidConnectionInClientHandlerProcess,
			OperatorRequestFailed
		};
	};
}

