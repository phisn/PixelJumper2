#pragma once

#include <Client/source/resource/ResourceBase.h>

#include <string>

namespace Resource
{
	class PlayerResource
		:
		public ResourceBase
	{
	public:
		struct
		{
			std::wstring UserName;
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
	};
}
