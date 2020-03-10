#pragma once

#include <random>

namespace Module::Random
{
	void Initialize();
	std::mt19937& GetEngine();

	void FillRandom(
		const int size,
		char* const mem);

	template <typename T>
	typename std::enable_if_t<std::is_class_v<T>, T> MakeRandom()
	{
		T value;
		FillRandom(sizeof(T), (char*) &value);
		return value;
	}

	template <typename T, typename = typename std::enable_if<std::is_arithmetic_v<T>>::type>
	typename std::enable_if_t<std::is_integral_v<T>, T> MakeRandom()
	{
		static std::uniform_int_distribution<unsigned long long> distribution(
			std::numeric_limits<T>::min(),
			std::numeric_limits<T>::max());

		return distribution(GetEngine());
	}

	template <typename T, typename = typename std::enable_if<std::is_arithmetic_v<T>>::type>
	typename std::enable_if_t<std::is_floating_point_v<T>, T> MakeRandom()
	{
		static std::uniform_real_distribution<T> distribution(
			std::numeric_limits<T>::min(),
			std::numeric_limits<T>::max());

		return distribution(GetEngine());
	}
}

