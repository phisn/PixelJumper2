#pragma once

#include <functional>
#include <vector>

namespace Editor
{
	template < 
		typename Input, 
		typename Output>
	class CacheElement
	{
		typedef std::function<bool(
			const Input* const input,
			Output* const output)
		> Worker;
		typedef std::pair<
			int, Worker
		> WorkerPair;
		typedef std::function<void(
			Output* const output)
		> Listener;
	public:
		CacheElement()
		{
		}

		virtual bool notify()
		{
			for (const WorkerPair& worker : workers)
				if (!worker.second(&input, &output))
				{
					return false;
				}

			for (const Listener& listener : listeners)
				listener(&output);

			return true;
		}

		void addWorker(
			const Worker worker, 
			const int priority)
		{
			decltype(workers)::const_iterator iterator = workers.cbegin();

			while (iterator != workers.cend())
			{
				if (iterator->first > priority)
				{
					break;
				}

				++iterator;
			}

			workers.insert(
				iterator,
				std::make_pair(priority, worker)
			);
		}

		void removeWorker(const Worker worker)
		{
			for (decltype(workers)::const_iterator iterator = workers.cbegin();
				iterator != workers.cend(); ++iterator)
				if (iterator->second.target<void>() == worker.target<void>())
				{
					workers.erase(iterator);
					break;
				}
		}

		void addListener(const Listener listener)
		{
			listeners.push_back(listener);
		}

		void removeListener(const Listener listener)
		{
			for (decltype(listeners)::const_iterator iterator = listeners.cbegin();
				iterator != listeners.cend(); ++iterator)
				if (iterator->target<void>() == listener.target<void>())
				{
					listeners.erase(iterator);
					break;
				}
		}

		Input* writeInput()
		{
			return &input;
		}

		const Output* readOutput() const
		{
			return &output;
		}

	private:
		Input input;
		Output output;

		std::vector<WorkerPair> workers;
		std::vector<Listener> listeners;
	};
}
