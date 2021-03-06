#pragma once

#include "ResourceCore/pipes/PipeBase.h"

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
		public NetworkMessage
	{
		Content content;

		virtual ~TrivialNetworkMessage()
		{
		}

		virtual bool load(Resource::ReadPipe* const pipe) override
		{
			return pipe->readValue(&content);
		}

		virtual bool save(Resource::WritePipe* const pipe) override
		{
			return pipe->writeValue(&content);
		}

		virtual void adopt(const Content& content)
		{
			memcpy(&(this->content), &content, sizeof(Content));
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

