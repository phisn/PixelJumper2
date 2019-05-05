#pragma once

#include <Client/source/menu/MenuProperty.h>

#include <functional>

namespace Menu
{
	class PercentProperty
		:
		public CustomProperty
	{
		typedef std::function<const float(const float)> Converter;
	public:
		PercentProperty(
			const Converter convertPercentToValue,
			const Converter convertValueToPercent)
			:
			convertPercentToValue(convertPercentToValue),
			convertValueToPercent(convertValueToPercent),
			value(0.f)
		{
		}

		float getPercent() const
		{
			return convertValueToPercent(value);
		}

		float getValue() const
		{
			return value;
		}

		void setPercent(const float percent)
		{
			value = convertPercentToValue(percent);
			onValueChanged();
		}

		void setValue(const float value)
		{
			this->value = value;
			onValueChanged();
		}

	private:
		float value;

		const Converter convertPercentToValue;
		const Converter convertValueToPercent;
	};
}