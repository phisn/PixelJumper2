#pragma once

#include <functional>
#include <vector>

#include <type_traits>

namespace Menu
{
	struct CustomProperty
	{
		virtual void onValueChanged() const noexcept = 0;
	};

	template <typename ListenerT>
	class PropertyBase
	{
		PropertyBase& operator=(PropertyBase&) = delete;
		PropertyBase& operator=(const PropertyBase&) = delete;

	public:
		typedef ListenerT Listener;

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

	protected:
		std::vector<Listener> listeners;
	};

	template <typename T>
	class ValuePropertyBase
		:
		public PropertyBase<std::function<void(
			T oldValue,
			T newValue)>>
	{
	public:
		ValuePropertyBase()
		{
		}

		ValuePropertyBase(const T value)
			:
			value(value)
		{
		}

		ValuePropertyBase& operator=(const T value)
		{
			return operator=<T>(value);
		}

		template <typename S>
		ValuePropertyBase& operator=(const S value)
		{
			setValue(value);
			return *this;
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
		// enable workarounds for indirect properties
		// like size
		virtual void setValue(const T value)
		{
			const T temp = std::move(this->value);
			this->value = T(value);

			valueChanged(
				std::move(temp)
			);
		}


		T value;

	private:
		void valueChanged(const T&& oldValue) const
		{
			for (const Listener& listener : listeners)
			{
				listener(std::move(oldValue), value);
			}
		}
	};

	template <typename T,
		bool UseCustomProperty = std::is_base_of_v<CustomProperty, T>
	>
	class Property
	{
	};

	template <typename T>
	class Property<T, false>
		:
		public ValuePropertyBase<T>
	{
	public:
		using ValuePropertyBase::ValuePropertyBase;

		const T* operator->() const
		{
			return &value;
		}

		const T& operator*() const
		{
			return value;
		}

		using ValuePropertyBase::operator const T;
		using ValuePropertyBase::operator=;
	};

	template <typename T>
	class Property<T*, false>
		:
		public ValuePropertyBase<T*>
	{
	public:
		using ValuePropertyBase::ValuePropertyBase;

		T* operator->() const
		{
			return value;
		}

		T& operator*() const
		{
			return *value;
		}

	//	using ValuePropertyBase::operator const T;
		using ValuePropertyBase::operator=;
	};

	template <typename T>
	class Property<T, true>
		:
		public PropertyBase<
			std::function<void(const T&)>
		>
	{
		class Value : public T
		{
		public:
			template <typename... Args>
			Value(Property<T, true>* const parent, Args... args)
				:
				T(args...),
				parent(parent)
			{
			}

			void onValueChanged() const noexcept override
			{
				parent->onValueChanged();
			}

		private:
			Property<T, true>* parent;
		};

	public:
		template <typename... Args>
		Property(Args... args)
			:
			value(this, args...)
		{
		}

		T* operator->()
		{
			return &value;
		}

		T& operator*()
		{
			return value;
		}

	public:
		void onValueChanged()
		{
			for (const Listener& listener : listeners)
			{
				listener(value);
			}
		}

		Value value;
	};
}