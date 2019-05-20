#pragma once

#include <Client/source/editor/manipulator/CacheElement.h>

#include <Client/source/editor/selector/SelectorCache.h>

namespace Editor
{
	class Cache
	{
	public:
		CacheElement<SelectorInput, SelectorOutput> selection;
	};
}
