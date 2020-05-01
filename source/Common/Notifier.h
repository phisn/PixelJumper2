#pragma once

#include "RandomModule.h"

#include <functional>
#include <vector>

namespace Util
{
	typedef uint64_t NotifierListenerID;

	template <typename NotifierT>
	class NotifyListenerContainer
	{
	public:
		typedef typename NotifierT::Listener Listener;

		NotifyListenerContainer(NotifierT& notifier)
			:
			notifier(notifier),
			id(NULL)
		{
		}

		NotifyListenerContainer(NotifierT& notifier, Listener listener)
			:
			notifier(notifier)
		{
			id = notifier.addListener(listener);
		}

		~NotifyListenerContainer()
		{
			notifier.popListener(id);
		}

		void setListener(Listener listener)
		{
			if (id)
				notifier.popListener(listener);

			id = notifier.addListener(listener);
		}

	private:
		NotifierT& notifier;
		NotifierListenerID id;
	};

	template <typename Parent, typename... ListenerParameter>
	class Notifier
	{
		friend Parent;

	public:
		typedef NotifyListenerContainer<
			Notifier<Parent, ListenerParameter...>
		> ListenerContainer;

		typedef std::function<void(ListenerParameter...)> Listener;
		typedef std::pair<Listener, NotifierListenerID> ListenerPair;

		NotifierListenerID addListener(Listener listener)
		{
			NotifierListenerID identifier = Module::Random::MakeRandom<NotifierListenerID>();

			if (!identifier)
				++identifier;

			addListener(listener, identifier);
			return identifier;
		}

		void addListener(Listener listener, NotifierListenerID identifier)
		{
			listeners.push_back(std::make_pair(listener, identifier));
		}

		void popListener(NotifierListenerID identifier)
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
		
		ListenerContainer makeListenerContainer()
		{
			return ListenerContainer{ *this };
		}

	private:
		virtual void notify(ListenerParameter... parameter)
		{
			for (const ListenerPair& listener : listeners)
				listener.first(parameter...);
		}

		std::vector<ListenerPair> listeners;
	};

	/*
	template <typename Parent, typename... ListenerParameter>
	class LazyNotifier
		:
		public Notifer<Parent, ListenerParameters...>
	{
	public:
		void process()
		{
		}

	private:
		bool
	};
	*/
}
