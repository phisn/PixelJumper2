#pragma once

#include <functional>
#include <vector>

#include <type_traits>

#include <SFML/System/Vector2.hpp>

namespace Util
{
	struct CustomProperty
	{
		virtual void onValueChanged() const noexcept = 0;
	};

	class PropertyBase
		:
		public Util::Notifier<PropertyBase>
	{
		PropertyBase& operator=(PropertyBase&) = delete;
		PropertyBase& operator=(const PropertyBase&) = delete;

		PropertyBase(PropertyBase&) = delete;
		PropertyBase(const PropertyBase&) = delete;

	public:
		typedef Util::Notifier<PropertyBase> NotifierT;

		using NotifierT::notify;
	};

	template <typename T>
	class ValuePropertyBase
		:
		public PropertyBase
	{
	public:
		typedef T Type;

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
			return getValue();
		}

		const T getValue() const
		{
			return value;
		}

		// enable workarounds for indirect properties
		// like size and lazy property
		virtual void setValue(const T value)
		{
			if (value != this->value)
			{
				this->value = T(value);
				this->valueChanged();
			}
		}

	protected:
		T value;
	};

	enum class BasicPropertyType
	{
		CommonProperty,
		CustomProperty,
		VectorProperty,
		ContainerMapProperty,
		ContainerVectorProperty,
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

	template <typename T>
	struct DeterminePropertyType<std::vector<T>, void>
	{
		static const BasicPropertyType type = BasicPropertyType::ContainerVectorProperty;
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

		Property& operator+=(const _T z)
		{
			if (z != 0)
			{
				value += z;
				notify(z);
			}

			return *this;
		}

		Property& operator-=(const _T z)
		{
			if (z != 0)
			{
				value -= z;
				notify();
			}

			return *this;
		}

		Property& operator*=(const E z)
		{
			if (z != 1)
			{
				value *= z;
				notify();
			}

			return *this;
		}

		Property& operator/=(const E z)
		{
			if (z != 1)
			{
				value /= z;
				notify();
			}

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
			if (x != value.x)
			{
				value.x = std::move(x);
				notify();
			}
		}

		void setY(const E y)
		{
			if (y != value.y)
			{
				value.y = std::move(y);
				notify();
			}
		}

		using ValuePropertyBase::setValue;
		void setValue(const E x, const E y)
		{
			if (x != value.x || y != value.y)
			{
				value.x = std::move(x);
				value.y = std::move(y);

				notify();
			}
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
			++value;
			notify();

			return *this;
		}

		T operator++(int)
		{
			T oldValue = value;
			++value;
			notify();
			return oldValue;
		}

		template <typename Z>
		Property& operator+=(const Z z)
		{
			if (z != 0)
			{
				value += (T)z;
				notify();
			}

			return *this;
		}

		Property& operator--()
		{
			--value;
			notify();

			return *this;
		}

		T operator--(int)
		{
			const T oldValue = value;
			--value;
			notify();
			return oldValue;
		}

		template <typename Z>
		Property& operator-=(const Z z)
		{
			if (z != 0)
			{
				value -= (T)z;
				notify();
			}

			return *this;
		}

		template <typename Z>
		Property& operator*=(const Z z)
		{
			if (z != 1)
			{
				value *= (T) z;
				notify();
			}

			return *this;
		}

		template <typename Z>
		Property& operator/=(const Z z)
		{
			if (z != 1)
			{
				value /= (T)z;
				notify();
			}

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
		public PropertyBase
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
				parent->notify();
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
		Value value;
	};

	template <typename T>
	class Property<std::vector<T>, BasicPropertyType::ContainerVectorProperty>
		:
		public PropertyBase,
		public std::vector<T>
	{
	public:
		using vector::vector;

		Property()
		{
		}

		void push_back(T value)
		{
			vector::push_back(value);
			notify();
		}

		void pop_back()
		{
			vector::pop_back();
			notify();
		}

		template <class... _Valty>
		decltype(auto) emplace_back(_Valty&&... _Val)
		{
			decltype(auto) temp = vector::emplace_back(_Val...);
			notify();
			return temp;
		}

		void clear()
		{
			vector::clear();
			vector::emplace_back();
			notify();
		}

		iterator erase(const_iterator _Where)
		{
			iterator temp = vector::erase(_Where);
			notify();
			return temp;
		}
	};
}

// need special for auto casting between
// sf::Vector2<Menu::Property<bool>> and
// sf::Vector2<bool> + easy constructors
namespace sf
{
	template <typename T>
	class Vector2<Util::Property<T>>
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
			Util::Property<U>
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

		Vector2<Util::Property<T>>& operator=(const Vector2<T>& vector)
		{
			const T temp = vector.x;

			y = vector.y;
			x = temp;

			return *this;
		}

		Util::Property<T> x, y;
	};

	template <typename T>
	bool operator==(const Vector2<Util::Property<T>>& v1, const Vector2<T>& v2)
	{
		return *v1.x == v2.x && *v1.y == v2.y;
	}

	template <typename T>
	bool operator==(const Vector2<T>& v2, const Vector2<Util::Property<T>>& v1)
	{
		return *v1.x == v2.x && *v1.y == v2.y;
	}

	template <typename T>
	bool operator!=(const Vector2<T>& v2, const Vector2<Util::Property<T>>& v1)
	{
		return !(v1 == v2);
	}

	template <typename T>
	bool operator!=(const Vector2<Util::Property<T>>& v1, const Vector2<T>& v2)
	{
		return !(v1 == v2);
	}
}
