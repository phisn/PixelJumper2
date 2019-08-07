#pragma once

#include <Client/source/editor/tile/TileTemplate.h>

#include <vector>

/*
	TileSeach (Event) [ex. Text Search | Type Search]
	    |
		v
  TileChoiceInput -> TileChoiceWorker (Worker)
	                        |
							v
					TileChoiceOutput -> tileChoiceRoot (Listener)
*/

namespace Editor
{
	class TileSearchInput
	{
	public:
		// ... ?
	};

	class TileSearchOutput
	{
	public:
		std::vector<TileTemplate*> tiles;
	};

	class TileChoiceInput
	{
	public:
		TileTemplate* selection;
	};

	class TileChoiceOutput
	{
	public:
		TileTemplate* selection;
	};
}