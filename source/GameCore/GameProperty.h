#pragma once

#include "Common/SpecialProperties.h"

namespace Game
{
	template <typename T>
	using Property = Util::LazyProperty<T>;

	// used to apply factor to value like input force

	/*
	template <typename T>
	struct DefinedProperty
	{
		static_assert(true, "Invalid type in DefinedProperty");
	};

	template <>
	class DefinedProperty<float>
		:
		public Util::LazyProperty<float>
	{
	public:
		using LazyProperty::LazyProperty;
		using LazyProperty::operator=;

		DefinedProperty(const float valueFactor)
			:
			valueFactor(valueFactor),
			LazyProperty()
		{
			setValue(1.f);
		}

		void setValue(const float value) override
		{
			LazyProperty::setValue(value * valueFactor);
		}

		void setValueRaw(float value)
		{
			LazyProperty::setValue(value);
		}

	private:
		const float valueFactor;
	};

	template <>
	class DefinedProperty<sf::Vector2f>
		:
		public Util::LazyProperty<sf::Vector2f>
	{
	public:
		using LazyProperty::LazyProperty;
		using LazyProperty::operator=;

		DefinedProperty(const float valueFactor)
			:
			valueFactor(valueFactor),
			LazyProperty()
		{
			setValue(sf::Vector2f(1.f, 1.f));
		}

		void setValue(sf::Vector2f value) override
		{
			LazyProperty::setValue(value * valueFactor);
		}

		void setValueRaw(sf::Vector2f value)
		{
			LazyProperty::setValue(value);
		}

	private:
		const float valueFactor;
	};
	*/

	template <typename T, typename Parent>
	using ReadOnlyProperty = Util::ReadOnlyPropertyContainer<T, Parent>;
}
