#pragma once

#include <Client/source/resource/Common.h>
#include <Client/source/resource/pipes/PipeBase.h>

#include <string>

namespace Resource
{
	class ResourceBase
	{
	public:
		virtual bool make(ReadPipe* const pipe) = 0;
		virtual bool save(WritePipe* const pipe) = 0;
	};

	struct Definition
	{
		Definition(
			const Magic magic,
			const std::wstring name)
			:
			magic(magic),
			name(name),
			path(DEFAULT_RES_PATH + name + L"/")
		{
		}

		const Magic magic;
		const std::wstring name, path;
	};
}
