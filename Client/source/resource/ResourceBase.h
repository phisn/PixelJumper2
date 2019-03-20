#pragma once

#include <Client/source/resource/pipes/PipeBase.h>

#include <functional>
#include <string>

#define EDIT // mark editable parameter
#define AUTO // mark non-editable parameter

namespace Resource
{
	class ResourceBase
	{
	public:
		virtual bool make(ReadPipe* const pipe) = 0; 
		virtual bool save(WritePipe* const pipe) = 0;

		// setup resource for save
		virtual bool setup() = 0;
	};
}
