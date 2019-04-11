#pragma once

#include <functional>
#include <vector>

#include <type_traits>

namespace Menu
{
	template <typename T>
	class Property
	{
		//typedef std::conditional<std::is_scalar_v<T>, T, T>::type ListenerType;  
		typedef std::function<void(
			T oldValue,
			T newValue)
		> Listener;
	public:
		Property()
		{
		}

		Property(const T value)
			:
			value(value)
		{
		}

		void addListener(const Listener listener)
		{
			listeners.push_back(listener);
		}

		bool popListener(const Listener listener)
		{
			decltype(listeners)::const_iterator iterator = listeners.cbegin();

			do
			{
				if (iterator->target<std::size_t>() == listener.target<std::size_t>())
				{
					listeners.erase(iterator);
					return true;
				}

			} while (++iterator != listeners.cend());

			return false;
		}

		Property& operator=(const T value)
		{
			return operator=<T>(value);
		}

		template <typename S>
		Property& operator=(const S value)
		{
			const T temp = std::move(this->value);
			this->value = T(value);

			valueChanged(
				std::move(temp)
			);

			return *this;
		}

		T* operator->()
		{
			return &value;
		}

		operator const T() const
		{
			return value;
		}

		const T getValue() const
		{
			return value;
		}

#ifdef DANGEROUS // TODO: ?
		T& writeValue()
		{
			return value;
		}
#endif

		void valueChanged(const T&& oldValue) const
		{
			for (const Listener& listener : listeners)
			{
				listener(std::move(oldValue), value);
			}
		}

	private:
		std::vector<Listener> listeners;
		T value;
	};
}