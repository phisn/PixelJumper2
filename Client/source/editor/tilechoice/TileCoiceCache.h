#pragma once

#include <Client/source/editor/template/TileTemplate.h>

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
	class TileChoiceInput
	{
	public:
		// ... ?
	};

	class TileChoiceOutput
	{
	public:
		std::vector<TileTemplate*> tiles;
	};
}