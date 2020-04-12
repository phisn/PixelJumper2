#pragma once

#include "Common/Common.h"
#include "ResourceBase.h"

#include <string>

namespace Resource
{
	// deprecated
	typedef Operator::UserID PlayerID;
	typedef sf::Uint32 WorldId;

	typedef sf::Uint32 RepresentationID;
	typedef WorldId WorldID;

	class PlayerResource
		:
		public ResourceBase
	{
	public:
		struct Content
		{
			Operator::UserID userID;

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

	// playerresource as a whole in the current game state
	// questionable because all data is stored in the database anyway
	// and transmition data is stored in message structs
	// one possibility would be to cache the player locally to allow
	// offline games with the playerstate
	// problem with this is that this would allow easy manipulation of
	// data

	/* 
	class PlayerResource
		:
		public ResourceBase
	{
	public:
		struct Content
		{
			PlayerID playerID;
			// RepresentationID representationID;

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
	*/
}
