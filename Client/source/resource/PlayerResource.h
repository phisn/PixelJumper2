#pragma once

#include <Client/source/resource/ResourceBase.h>
#include <Operator/source/Common.h>

#include <string>

namespace Resource
{
	typedef Operator::UserID PlayerID;
	typedef sf::Uint32 RepresentationID;
	typedef sf::Uint32 WorldId;

	class PlayerResource
		:
		public ResourceBase
	{
	public:
		struct Content
		{
			PlayerID playerID;

		} content;

		std::wstring username;
		
		bool make(ReadPipe* const pipe) override
		{
			if (!pipe->readValue(&content) ||
				!pipe->readString(&username))
			{
				return false;
			}


		}

		bool save(WritePipe* const pipe) override
		{
			return pipe->writeValue(&content)
				&& pipe->writeString(&username)
				&& classic.save(pipe);
		}

		bool setup() override
		{
			return content.playerID != NULL && classic.setup();
		}

		bool validate() override
		{
			return content.playerID != NULL && classic.validate();
		}
	};
}
