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
			RepresentationID representationID;

		} content;

		std::string username;
		
		bool make(ReadPipe* const pipe) override
		{
			return pipe->readValue(&content) 
				&& pipe->readString(&username);
		}

		bool save(WritePipe* const pipe) override
		{
			return pipe->writeValue(&content)
				&& pipe->writeString(&username);
		}

		bool setup() override
		{
			return content.playerID != NULL;
		}

		bool validate() override
		{
			return content.playerID != NULL;
		}
	};
}
