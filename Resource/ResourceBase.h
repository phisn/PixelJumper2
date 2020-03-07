#pragma once

#include "pipes/PipeBase.h"

#include <functional>
#include <string>

namespace Resource
{
	class ResourceBase
	{
	public:
		virtual bool make(ReadPipe* const pipe) = 0; 
		virtual bool save(WritePipe* const pipe) = 0;

		virtual bool setup() = 0; // before save
		virtual bool validate() = 0; // after make
	};
}
