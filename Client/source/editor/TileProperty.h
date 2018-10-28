#pragma once

#include <string>

namespace Editor
{
	enum class Type
	{
		Bool,
		Integer,
		Number,
		String
	};

	struct TileProperty
	{
		~TileProperty()
		{
			delete value;
		}

		Type type;

		void* value;
	};
}