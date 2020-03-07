#pragma once

#include "ComponentResource.h"

namespace Resource
{
	struct CommonExitComponent
		:
		public ComponentResource
	{
		bool make(ReadPipe* const pipe) override
		{
			return true;
		}

		bool save(WritePipe* const pipe) override
		{
			return true;
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
