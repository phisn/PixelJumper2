#pragma once

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
}
