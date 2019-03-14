#pragma once

#include <Client/source/resource/pipes/PipeBase.h>

#include <cassert>
#include <functional>
#include <unordered_set>

namespace Game
{
	struct GameState
	{
		virtual bool writeState(Resource::WritePipe* const writePipe) = 0;
		virtual bool readState(Resource::ReadPipe* const readPipe) = 0;
	};

	template <typename T>
	class PropertyWriter
	{
		typedef std::function<void(const T)> Listener;
	public:
		PropertyWriter(T& value)
			:
			value(value)
		{
		}

		PropertyWriter<T>& operator=(const T value)
		{
			const T oldValue = this->value;
			this->value = value;

			for (const Listener& listener : listeners)
			{
				listener(oldValue);
			}

			return *this;
		}

		void addListener(const Listener listener)
		{
			listeners.push_back(listener);
		}

		void removeListener(const Listener listener)
		{
			for (decltype(listeners)::iterator iterator = listeners.begin()
				; iterator != listeners.end(); ++iterator)
				if (iterator->target<void>() == listener.target<void>())
				{
					listeners.erase(iterator);

					break;
				}
		}

	private:
		T& value;
		std::vector<Listener> listeners;
	};

}
