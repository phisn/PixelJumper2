#pragma once

#include <Client/source/menu/MenuProperty.h>

namespace Menu
{
	template <typename T>
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
			automatic.setValue(false);
			Property<T>::operator=(value);
			return *this;
		}

		// use setValue for automatic define
	};

	template <typename T>
	class LazyListenerProperty
		:
		public Property<T>
	{
	public:
		typedef std::function<void(const T&)> LazyListener;

		void addLazyListener(LazyListener listener)
		{
			lazyListeners.push_back(listener);
		}

		void updateLazy()
		{
			for (const LazyListener& listener : lazyListeners)
				listener(**this);
		}

	private:
		std::vector<LazyListener> lazyListeners;
	};

	template <typename Parent, typename T>
	class ReadOnlyProperty
		:
		private Property<T>
	{
		friend Parent;

		using Property::operator=;
	public:
		static_assert(
			!std::is_base_of_v<CustomProperty, T>,
			"ReadOnlyProperty cant use CustomProperty"
		);

		using Property::Property;
		using Property::operator->;
		using Property::operator*;

		using Property::addListener;
		using Property::popListener;

		using Property::getValue;
	};
}
