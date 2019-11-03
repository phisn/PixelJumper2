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

	// used to add listener indpendently from
	// template type
	class IndependentPropertyBase
	{
	public:
		typedef size_t IndependentListenerId;
		typedef std::function<void()> IndependentListener;

		IndependentListenerId addIndependentListener(const IndependentListener listener)
		{
			const IndependentListenerId nextId = ++lastId;
			independentListeners.push_back(std::make_pair(nextId, listener));
			return nextId;
		}

		bool removeIndependentListener(const IndependentListenerId id)
		{
			for (IndependentListenerContainer::const_iterator iterator = independentListeners.cbegin()
			   ; iterator != independentListeners.cend(); iterator++)
				if (iterator->first == id)
				{
					independentListeners.erase(iterator);
					return true;
				}

			return false;
		}

	protected:
		void callIndependentListeners() const
		{
			for (const IndependentListenerPair& listener : independentListeners)
				listener.second();
		}

	private:
		static IndependentListenerId lastId;

		typedef std::pair<IndependentListenerId, IndependentListener> IndependentListenerPair;
		typedef std::vector<IndependentListenerPair> IndependentListenerContainer;

		IndependentListenerContainer independentListeners;
	};

	template <typename ListenerT>
	class PropertyBase
		:
		public IndependentPropertyBase
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
		typedef T Type;

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
		// like size and lazy property
		virtual void setValue(const T value)
		{
			const T temp = std::move(this->value);
			this->value = T(value);

			valueChanged(
				std::move(temp)
			);
		}

	protected:
		void valueChanged(const T&& oldValue) const
		{
			if (value == oldValue)
			{
				return;
			}

			for (const Listener& listener : listeners)
			{
				listener(std::move(oldValue), value);
			}

			callIndependentListeners();
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
		Property()
			:
			ValuePropertyBase()
		{
		}

		Property(const sf::Vector2<E> value)
			:
			ValuePropertyBase(value)
		{
		}

		/*explicit Property(const E x, const E y)
			:
			ValuePropertyBase(
				sf::Vector2<E>(x, y)
			)
		{
		}*/

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

			callIndependentListeners();
		}

		Value value;
	};
}

// need special for auto casting between
// sf::Vector2<Menu::Property<bool>> and
// sf::Vector2<bool> + easy constructors
namespace sf
{
	template <typename T>
	class Vector2<Menu::Property<T>>
	{
	public:
		Vector2()
			:
			Vector2(0, 0)
		{
		}

		Vector2(T x, T y)
			:
			x(x),
			y(y)
		{
		}

		Vector2(Vector2<T> vector)
			:
			x(vector.x),
			y(vector.y)
		{
		}

		template <typename U>
		explicit Vector2(const Vector2<U>& vector)
			:
			x(vector.x),
			y(vector.y)
		{
		}

		template <typename U>
		explicit Vector2(const Vector2<
				Menu::Property<U>
			>& vector)
			:
			x(*vector.x),
			y(*vector.y)
		{
		}

		operator Vector2<bool>()
		{
			return Vector2<bool>(*x, *y);
		}

		Vector2<Menu::Property<T>>& operator=(const Vector2<T>& vector)
		{
			const T temp = vector.x;

			y = vector.y;
			x = temp;

			return *this;
		}

		Menu::Property<T> x, y;
	};

	template <typename T>
	bool operator==(const Vector2<Menu::Property<T>>& v1, const Vector2<T>& v2)
	{
		return *v1.x == v2.x && *v1.y == v2.y;
	}

	template <typename T>
	bool operator==(const Vector2<T>& v2, const Vector2<Menu::Property<T>>& v1)
	{
		return *v1.x == v2.x && *v1.y == v2.y;
	}

	template <typename T>
	bool operator!=(const Vector2<T>& v2, const Vector2<Menu::Property<T>>& v1)
	{
		return !(v1 == v2);
	}

	template <typename T>
	bool operator!=(const Vector2<Menu::Property<T>>& v1, const Vector2<T>& v2)
	{
		return !(v1 == v2);
	}
}
