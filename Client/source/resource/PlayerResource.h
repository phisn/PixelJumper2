#pragma once

#include <Client/source/resource/ResourceBase.h>

#include <string>

namespace Resource
{
	typedef sf::Uint64 PlayerID;

	class ConnectionResource
		:
		public ResourceBase
	{
	public:
		struct Content
		{
			PlayerID playerId;
			std::wstring userName;

		} content;

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

	class PlayerResource
		:
		public ResourceBase
	{
	public:
		ConnectionResource connection;
		
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
