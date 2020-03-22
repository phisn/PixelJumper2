#pragma once

#include <functional>
#include <vector>

namespace Util
{
	template <typename Parent, typename... ListenerParameter>
	class Notifier
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
			decltype(listeners)::const_iterator iterator = listeners.cbegin();

			while (iterator != listeners.cend())
			{
				if (iterator->second == identifier)
				{
					iterator = listeners.erase(iterator);
				}
				else
				{
					++iterator;
				}
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
