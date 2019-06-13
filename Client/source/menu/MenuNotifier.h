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

		void addListener(Listener listener)
		{
			listeners.push_back(listener);
		}

		void popListener(Listener listener)
		{
			if (listeners.size() == 0)
			{
				return;
			}

			decltype(listeners)::const_iterator iterator = listeners.cbegin();

			while (iterator->target<void>() != listener.target<void>())
			{
				if (iterator == listeners.cend())
				{
					return;
				}

				++iterator;
			}

			listeners.erase(iterator);
		}

		void clearListener()
		{
			listeners.clear();
		}

	private:
		void notify(ListenerParameter... parameter)
		{
			for (const Listener& listener : listeners)
				listener(parameter...);
		}

		std::vector<Listener> listeners;
	};
}
