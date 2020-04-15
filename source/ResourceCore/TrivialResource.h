#pragma once

#include "ResourceBase.h"

namespace Resource
{
	// made everything virtual to allow partly implementation of
	// trivial resources and specialize methods
	template <typename Content>
	struct TrivialResource
		:
		public ResourceBase
	{
		Content content;

		virtual ~TrivialResource()
		{
		}

		virtual bool make(Resource::ReadPipe* const pipe) override
		{
			return pipe->readValue(&content);
		}

		virtual bool save(Resource::WritePipe* const pipe) override
		{
			return pipe->writeValue(&content);
		}

		virtual void adopt(const Content& content)
		{
			memcpy(&(this->content), &content, sizeof(Content));
		}
	};
}
