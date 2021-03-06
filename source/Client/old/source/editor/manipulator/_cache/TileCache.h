#pragma once

#include <Client/source/editor/template/TileTemplate.h>

namespace Editor
{
	namespace TileCache
	{
		struct Input
		{
			TileTemplate* tile;
		};

		struct Output
		{
			TileTemplate* tile;
		};

		// currently doesnt need output
		// & doesnt need element
	}
}