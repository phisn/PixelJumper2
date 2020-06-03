#pragma once

#include "EditorDatabase.h"
#include "EditorDataset.h"

#include <SFML/Window.hpp>

#include <map>
#include <typeindex>
#include <typeinfo>

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

	class WindowManager
	{
		struct WindowEntry
		{
			std::vector<void*> raw;
			std::vector<EditorWindow*> window;
		};

		static WindowManager* instance;

	public:
		static WindowManager* Instance()
		{
			return instance;
		}

		WindowManager()
		{
			assert(instance == NULL);
			instance = this;
		}

		~WindowManager()
		{
			assert(instance != NULL);
			instance = NULL;

			for (decltype(windows)::value_type& pair : windows)
				for (EditorWindow* window : pair.second.window)
				{
					delete window;
				}
		}

		template <typename T>
		void makeWindow(T* window)
		{
			WindowEntry& entry = windows[typeid(T)];

			entry.raw.push_back((T*) window);
			entry.window.push_back(window);

			windowsQuick.push_back(window);
		}

		template <typename T>
		void removeWindow(T* window)
		{
			removeWindow((void*) window, typeid(T));
		}

		void removeWindow(void* raw, std::type_index index)
		{
			WindowEntry& entry = windows[index];
			for (int i = 0; i < entry.raw.size(); ++i)
				if (entry.raw[i] == raw)
				{
					decltype(windowsQuick)::iterator iterator = std::find(
						windowsQuick.begin(),
						windowsQuick.end(), entry.window[i]);

					assert(iterator != windowsQuick.end());
					delete *iterator;
					windowsQuick.erase(iterator);

					entry.raw.erase(entry.raw.begin() + i);
					entry.window.erase(entry.window.begin() + i);

					for (int j = i; j < entry.raw.size(); ++j)
					{
						entry.window[j]->changeWindowIndex(j);
					}

					return;
				}

			assert(false);
		}

		void notifyDatabaseEvent(DatabaseEvent& event)
		{
			for (EditorWindow* window : windowsQuick)
				window->onDatabaseEvent(event);
		}

		template <typename T>
		const std::vector<T*>& getWindowsType()
		{
			return (std::vector<T*>) (windows[typeid(T)].raw);
		}

		std::vector<EditorWindow*> getWindows() const
		{
			return windowsQuick;
		}

	private:
		std::vector<EditorWindow*> windowsQuick;
		std::map<std::type_index, WindowEntry> windows;
	};
}
