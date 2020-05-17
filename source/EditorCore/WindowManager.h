#pragma once

#include "EditorWindow.h"

#include <map>
#include <typeindex>
#include <typeinfo>

namespace Editor
{
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

		void removeWindow(void* raw)
		{
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
					windowsQuick.erase(iterator);

					entry.raw.erase(entry.raw.begin() + i);
					entry.window.erase(entry.window.begin() + i);
				}

			assert(false);
		}

		template <typename T>
		const std::vector<T*>& getWindowList()
		{
			return (std::vector<T*>) (windows[typeid(T)].raw);
		}

	private:
		std::vector<EditorWindow*> windowsQuick;
		std::map<std::type_index, WindowEntry> windows;
	};
}
