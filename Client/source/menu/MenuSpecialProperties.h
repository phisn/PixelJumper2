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
}
