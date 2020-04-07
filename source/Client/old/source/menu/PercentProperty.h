#pragma once

#include <Client/source/menu/MenuProperty.h>

#include <functional>
#include <utility>

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
			const float result = convertValueToPercent(value);
			return std::isnan(result) ? 0.f : result;
		}

		float getValue() const
		{
			return value;
		}

		void setPercent(const float percent)
		{
			const float result = convertPercentToValue(percent);
			value = std::isnan(result) ? 0.f : result;

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