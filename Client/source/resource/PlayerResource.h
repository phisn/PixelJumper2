#pragma once

#include <Client/source/resource/ResourceBase.h>
#include <Client/source/resource/WorldResource.h>

#include <string>

namespace Resource
{
	typedef sf::Uint64 PlayerID;
	typedef sf::Uint32 RepresentationID;

	class PlayerClassicResource
		:
		public ResourceBase
	{
	public:
		struct Content
		{
			
		} content;

		std::vector<RepresentationID> representations;
		std::vector<Resource::WorldId> activeWorldSpawns;

		bool make(ReadPipe* const pipe) override
		{
			return pipe->readVector(&representations)
				&& pipe->readVector(&activeWorldSpawns);
		}

		bool save(WritePipe* const pipe) override
		{
			return pipe->writeVector(&representations)
				&& pipe->writeVector(&activeWorldSpawns);
		}

		bool setup() override
		{
			return true;
		}

		bool validate() override
		{
			return true;
		}
	};

	class PlayerResource
		:
		public ResourceBase
	{
	public:
		PlayerClassicResource classic;

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
