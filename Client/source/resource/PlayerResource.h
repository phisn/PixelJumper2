#pragma once

#include <Client/source/resource/ResourceBase.h>

#include <string>

namespace Resource
{
	typedef sf::Uint64 PlayerID;

	class PlayerClassicResource
	{
	public:
	};

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
		}

		bool save(WritePipe* const pipe) override
		{
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
}
