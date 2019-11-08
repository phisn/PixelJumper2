#pragma once

#include <Client/source/resource/ResourceBase.h>

#include <string>

namespace Resource
{
	typedef sf::Uint64 PlayerId;

	class ConnectionResource
		:
		public ResourceBase
	{
	public:
		struct
		{
			EDIT PlayerId playerId;

		} HeaderIntro;

		struct
		{
			EDIT std::wstring userName;

		} HeaderAuth;

	private:
		bool make(ReadPipe* const pipe) override
		{

		}

		bool save(WritePipe* const pipe) override
		{

		}

		bool setup() override
		{

		}
	};

	class PlayerResource
		:
		public ResourceBase
	{
	public:
		ConnectionResource connection;


	private:
		bool make(ReadPipe* const pipe) override
		{

		}

		bool save(WritePipe* const pipe) override
		{

		}

		bool setup() override
		{

		}
	};
}
