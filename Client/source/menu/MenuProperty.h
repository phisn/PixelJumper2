#pragma once

#include <functional>
#include <vector>

#include <type_traits>

#include <SFML/System/Vector2.hpp>

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

		PropertyBase(PropertyBase&) = delete;
		PropertyBase(const PropertyBase&) = delete;

	public:
		typedef ListenerT Listener;

		PropertyBase() = default;

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
		T value;

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

		// sync
		// allow to sync properties (be always the same)
		void sync(ValuePropertyBase<T>& other)
		{
			if (ValuePropertyBase<T>* const pOther = &other; pOther != currentSync)
			{
				desync(); // check inside
				currentSync = pOther;

				if (other.currentSync != this)
				{
					other.sync(*this);
					updateSync();
				}
			}
		}

		void desync()
		{
			if (currentSync)
			{
				currentSync->desync();
				currentSync = NULL;
			}
		}

		ValuePropertyBase<T>& getSync() const
		{
			return *currentSync;
		}

		bool isSync() const
		{
			return currentSync == NULL;
		}

	private:
		void updateSync() const
		{
			if (value != currentSync->getValue())
			{
				currentSync->setValue(value);
			}
		}

		ValuePropertyBase<T>* currentSync = NULL;

		// value
	protected:
		void valueChanged(const T&& oldValue) const
		{
			if (currentSync)
			{
				updateSync();
			}

			for (const Listener& listener : listeners)
			{
				listener(std::move(oldValue), value);
			}
		}
	};

	enum class BasicPropertyType
	{
		CommonProperty,
		CustomProperty,
		VectorProperty,
		ArithmeticProperty,
		PointerProperty
	};

	template <typename T, typename Eneable = void>
	struct DeterminePropertyType
	{
		static const BasicPropertyType type = BasicPropertyType::CommonProperty;
	};

	template <typename T>
	struct DeterminePropertyType<T, std::enable_if_t<std::is_arithmetic_v<T>>>
	{
		static const BasicPropertyType type = BasicPropertyType::ArithmeticProperty;
	};

	template <typename T>
	struct DeterminePropertyType<T, std::enable_if_t<std::is_pointer_v<T>>>
	{
		static const BasicPropertyType type = BasicPropertyType::PointerProperty;
	};

	template <typename T>
	struct DeterminePropertyType<T, std::enable_if_t<std::is_base_of_v<CustomProperty, T>>>
	{
		static const BasicPropertyType type = BasicPropertyType::CustomProperty;
	};

	template <typename T>
	struct DeterminePropertyType<sf::Vector2<T>, void>
	{
		static const BasicPropertyType type = BasicPropertyType::VectorProperty;
	};

	template <typename T, 
		BasicPropertyType = DeterminePropertyType<T>::type
	>
	class Property
	{
	};

	template <typename T>
	class Property<T, BasicPropertyType::CommonProperty>
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

	template <typename E>
	class Property<sf::Vector2<E>, BasicPropertyType::VectorProperty>
		:
		public ValuePropertyBase<sf::Vector2<E>>
	{
		using _T = sf::Vector2<E>;

		Property(Property&) = delete;
		Property(const Property&) = delete;

	public:
		using ValuePropertyBase::ValuePropertyBase;

		/*explicit Property(const E x, const E y)
		{
			value.x = std::move(x);
			value.y = std::move(y);
		}*/

		Property& operator+=(const _T z)
		{
			const _T oldValue = value;
			value += z;
			valueChanged(std::move(oldValue));

			return *this;
		}

		Property& operator-=(const _T z)
		{
			const _T oldValue = value;
			value -= z;
			valueChanged(std::move(oldValue));

			return *this;
		}

		Property& operator*=(const E z)
		{
			const _T oldValue = value;
			value *= z;
			valueChanged(std::move(oldValue));

			return *this;
		}

		Property& operator/=(const E z)
		{
			const _T oldValue = value;
			value /= z;
			valueChanged(std::move(oldValue));

			return *this;
		}

		const _T* operator->() const
		{
			return &value;
		}

		const _T& operator*() const
		{
			return value;
		}

		void setX(const E x)
		{
			const _T oldValue = value;
			value.x = std::move(x);
			valueChanged(std::move(oldValue));
		}

		void setY(const E y)
		{
			const _T oldValue = value;
			value.y = std::move(y);
			valueChanged(std::move(oldValue));
		}

		using ValuePropertyBase::setValue;
		void setValue(const E x, const E y)
		{
			const _T oldValue = value;
			
			value.x = std::move(x);
			value.y = std::move(y);

			valueChanged(std::move(oldValue));
		}

		using ValuePropertyBase::operator const _T;
		using ValuePropertyBase::operator=;
	};

	template <typename T>
	class Property<T, BasicPropertyType::ArithmeticProperty>
		:
		public Property<T, BasicPropertyType::CommonProperty>
	{
	public:
		Property()
			:
			Property<T, BasicPropertyType::CommonProperty>()
		{
		}

		Property(const T value)
			:
			Property<T, BasicPropertyType::CommonProperty>(value)
		{
		}

		Property& operator++()
		{
			const T oldValue = value;
			++value;
			valueChanged(std::move(oldValue));

			return *this;
		}

		T operator++(int)
		{
			const T oldValue = value;
			++value;
			valueChanged(std::move(oldValue));

			return oldValue;
		}

		template <typename Z>
		Property& operator+=(const Z z)
		{

			const T oldValue = value;
			value += (T) z;
			valueChanged(std::move(oldValue));

			return *this;
		}

		Property& operator--()
		{
			const T oldValue = value;
			--value;
			valueChanged(std::move(oldValue));

			return *this;
		}

		T operator--(int)
		{
			const T oldValue = value;
			--value;
			valueChanged(std::move(oldValue));

			return oldValue;
		}

		template <typename Z>
		Property& operator-=(const Z z)
		{

			const T oldValue = value;
			value -= (T)z;
			valueChanged(std::move(oldValue));

			return *this;
		}

		template <typename Z>
		Property& operator*=(const Z z)
		{

			const T oldValue = value;
			value *= (T)z;
			valueChanged(std::move(oldValue));

			return *this;
		}

		template <typename Z>
		Property& operator/=(const Z z)
		{

			const T oldValue = value;
			value /= (T)z;
			valueChanged(std::move(oldValue));

			return *this;
		}

		using Property<T, BasicPropertyType::CommonProperty>::operator->;
		using Property<T, BasicPropertyType::CommonProperty>::operator*;

		using Property<T, BasicPropertyType::CommonProperty>::operator const T;
		using Property<T, BasicPropertyType::CommonProperty>::operator=;
	};

	template <typename T>
	class Property<T*, BasicPropertyType::PointerProperty>
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
	class Property<T, BasicPropertyType::CustomProperty>
		:
		public PropertyBase<
			std::function<void(const T&)>
		>
	{
		class Value : public T
		{
		public:
			template <typename... Args>
			Value(Property<T, BasicPropertyType::CustomProperty>* const parent, Args... args)
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
			Property<T, BasicPropertyType::CustomProperty>* parent;
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

void _()
{
	Menu::Property<float> pf1;
	Menu::Property<float> pf2;

	pf1 = 5;
	pf1.sync(pf2);

	// pf2 == 5

	pf2 = 3;

	// pf1 == 3

	pf2.desync();
	pf2 = 5;

	// pf1 == 3
	// pf2 == 5
}