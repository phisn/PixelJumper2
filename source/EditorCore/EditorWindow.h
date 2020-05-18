#pragma once

#include "EditorDatabase.h"
#include "EditorDataset.h"

#include <SFML/Graphics.hpp>

namespace Editor
{
	enum class DatabaseEventType
	{
		Create,
		Change,
		Remove
	};

	struct DatabaseEvent
	{
		DatabaseEventType type;
		DatabaseTable table;

		union
		{
			ContextEvent context;
			WorldEvent world;
			TransitiveEvent transitive;
			TileEvent tile;

		} data;
	};

	class EditorWindow
	{
		friend class WindowManager;

	public:
		~EditorWindow()
		{
		}

		virtual void onProcess() = 0;
		virtual void onDatabaseEvent(DatabaseEvent event) = 0;
		virtual void onEvent(sf::Event event) = 0;

	protected:
		virtual void changeWindowIndex(int index) = 0;
	};
}
