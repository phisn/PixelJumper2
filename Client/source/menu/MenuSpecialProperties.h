#pragma once

#include <Client/source/menu/MenuProperty.h>

namespace Menu
{
	template <typename T, BasicPropertyType = DeterminePropertyType<T>::type>
	class AutomaticProperty
		:
		public Property<T>
	{
	public:
		using Property::Property;
		using Property::operator->;
		using Property::operator*;

		Property<bool> automatic{ true };

		template <typename S>
		AutomaticProperty& operator=(const S value)
		{
			if (automatic)
			{
				automatic.setValue(false);
			}

			Property<T>::operator=(value);
			return *this;
		}

		// use setValue for automatic define
	};

	template <typename E>
	class AutomaticProperty<sf::Vector2<E>, BasicPropertyType::VectorProperty>
		:
		public Property<sf::Vector2<E>, BasicPropertyType::VectorProperty>
	{
		typedef sf::Vector2<E> T;
		typedef Property<T, BasicPropertyType::VectorProperty> Parent;

	public:
		using Property::Property;
		using Property::operator->;
		using Property::operator*;

		sf::Vector2<Property<bool>> automatic{ { true }, { true } };

		template <typename S>
		AutomaticProperty& operator=(const S value)
		{
			if (automatic.x || automatic.y)
			{
				sf::Vector2<bool> result = automatic;

				if (result.x && this->value.x != value.x)
					result.x = false;

				if (result.y && this->value.y != value.y)
					result.y = false;

				if (result != (sf::Vector2<bool>) automatic)
				{
					automatic = result;
				}
			}

			Property<T>::operator=(value);
			return *this;
		}

		bool isAutomatic() const
		{
			return automatic.x || automatic.y;
		}

		// use setValue for automatic define
	};

	template <typename T>
	class LazyProperty
		:
		public Property<T>
	{
	public:
		using Property<T>::operator=;
		
		void setValue(const T value) override
		{
			if (!needsUpdate)
			{
				oldValue = std::move(this->value);
				needsUpdate = true;
			}

			this->value = T(value);
		}

		bool update(const bool force = false) const
		{
			if (force || needsUpdate)
			{
				valueChanged(std::move(oldValue));

				needsUpdate = false;
				return true;
			}

			return false;
		}

	private:
		T oldValue;
		mutable bool needsUpdate;
	};

	template <typename Property, typename Parent>
	class ReadOnlyPropertyContainer
		:
		private Property
	{
		friend Parent;

		using Property::operator=;
	public:
		static_assert(
			!std::is_base_of_v<CustomProperty, Property::Type>,
			"ReadOnlyProperty cant use CustomProperty"
		);

		using Property::Property;
		using Property::operator->;
		using Property::operator*;

		using Property::addListener;
		using Property::popListener;

		using Property::getValue;
	};

	// fix c3881 can only inherit constructor from direct base
	template <typename T, typename Parent>
	class ReadOnlyPropertyContainer<LazyProperty<T>, Parent>
		:
		private LazyProperty<T>
	{
		typedef LazyProperty<T> Base;
		friend Parent;

		using Property::operator=;
	public:
		static_assert(
			!std::is_base_of_v<CustomProperty, Base::Type>,
			"ReadOnlyProperty cant use CustomProperty"
		);

		using Base::Base;
		using Base::operator->;
		using Base::operator*;

		using Base::addListener;
		using Base::popListener;

		using Base::getValue;
	};
}
