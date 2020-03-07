#pragma once

#include "ResourceBase.h"

namespace Resource
{
	template <typename Content>
	struct TrivialResource
		:
		public ResourceBase,
		public Content
	{
		bool make(Resource::ReadPipe* const pipe) override
		{
			return pipe->readValue(this);
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return pipe->writeValue(this);
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
