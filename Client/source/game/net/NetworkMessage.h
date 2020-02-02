#pragma once

#include <Client/source/resource/pipes/PipeBase.h>

namespace Game::Net
{
	struct NetworkMessage
	{
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
			Error,
			_Offset
		};
	};

	class CommonErrorMessage
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
			InvalidConnectionInClientHandlerProcess
		};
	};
}
