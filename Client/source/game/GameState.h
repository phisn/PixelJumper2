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
	class StateProperty
	{
	public:
		typedef std::function<void(const T oldValue)> Listener;

		StateProperty(T& value)
		{
		}

		void callListeners(const T oldValue)
		{
			for (const Listener& listener : listeners)
			{
				listener(oldValue);
			}
		}

		void removeListener(
			const Listener listener)
		{
			listeners.erase(listener);
		}

		void addListener(
			const Listener listener)
		{
			listeners.insert(listener);
		}

		StateProperty& operator=(const T value)
		{
			const T oldValue = this->value;
			this->value = value;

			callListeners(value);

			return *this;
		}

		T getValue() const
		{
			return value;
		}

		operator const T() const
		{
			return value;
		}

	private:
		std::unordered_set<Listener> listeners;
		T& value;
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

		T& operator=(const T value)
		{
			const T oldValue = this->value;
			this->value = value;

			for (const Listener& listener : listeners)
			{
				listener(oldValue);
			}
		}

		void addListener(const Listener listener)
		{
			listeners.insert(listener);
		}

		void removeListener(const Listener listener)
		{
			listeners.erase(listener);
		}

	private:
		T& value;
		std::unordered_set<Listener> listeners;
	};

}
