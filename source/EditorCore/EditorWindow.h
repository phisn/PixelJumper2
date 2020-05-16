#pragma once

#include "EditorDatabase.h"

#include <SFML/Graphics.hpp>

namespace Editor
{
	struct DatabaseEvent
	{
		DatabaseTable table;
		uint64_t id;
	};

	struct EditorWindow
	{
		virtual void onProcess() = 0;
		virtual void onDatabaseEvent(DatabaseEvent event) = 0;
		virtual void onEvent(sf::Event event) = 0;
	};
}
