#pragma once


#include <Client/source/editor/manipulator/CacheElement.h>

#include <Client/source/editor/selector/SelectorCache.h>
#include <Client/source/editor/tilechoice/TileCoiceCache.h>

namespace Editor
{
	class Cache
	{
	public:
		CacheElement<SelectorInput, SelectorOutput> selection;
		CacheElement<TileSearchInput, TileSearchOutput> tileSearch;
		CacheElement<TileChoiceInput, TileChoiceOutput> tileChoice;
	};
}