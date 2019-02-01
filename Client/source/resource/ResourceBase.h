#pragma once

#include <Client/source/resource/Common.h>
#include <Client/source/resource/pipes/PipeBase.h>

#include <functional>
#include <string>

#define EDIT // Editable parameter
#define AUTO // Non-Editable parameter

namespace Resource
{
	class ResourceBase
	{
	public:
		virtual bool make(ReadPipe* const pipe) = 0;
		virtual bool save(WritePipe* const pipe) = 0;
	};

	struct ResourceDefinition
	{
		typedef std::function<Resource::ResourceBase*()> Callback;

		ResourceDefinition(
			const Magic magic,
			const std::wstring name,
			const Callback callback)
			:
			magic(magic),
			name(name),
			path(DEFAULT_RES_PATH + name + L"/"),
			callback(callback)
		{
		}

		const Callback callback;
		const Magic magic;
		const std::wstring name, path;
	};
}
