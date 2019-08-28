#pragma once

#include <Client/source/resource/ResourceBase.h>

#include <string>

namespace Resource
{
	typedef sf::Uint64 PlayerId;

	class PlayerResource
		:
		public ResourceBase
	{
	public:
		struct
		{
			PlayerId playerId;

		} HeaderIntro;

		struct
		{
			std::wstring userName;


		} HeaderAuth;

	private:
		bool make(
			ReadPipe* const pipe) override
		{

		}

		bool save(
			WritePipe* const pipe) override
		{

		}

		bool setup() override
		{

		}
	};
}
