#pragma once

#include <functional>
#include <vector>

namespace Menu
{
	template <typename Parent, typename... ListenerParameter>
	class MenuNotifier
	{
		friend Parent;

	public:
		typedef std::function<void(ListenerParameter...)> Listener;
		typedef std::pair<Listener, size_t> ListenerPair;

		void addListener(Listener listener, const size_t identifier = 0)
		{
			listeners.push_back(std::make_pair(listener, identifier));
		}

		void popListener(const size_t identifier)
		{
			for (decltype(listeners)::const_iterator iterator = listeners.cbegin();
				iterator != listeners.cend(); ++iterator)
				if (iterator->second == identifier)
				{
					listeners.erase();
				}
		}

		void clearListener()
		{
			listeners.clear();
		}

	private:
		void notify(ListenerParameter... parameter)
		{
			for (const ListenerPair& listener : listeners)
				listener.first(parameter...);
		}

		std::vector<ListenerPair> listeners;
	};
}
