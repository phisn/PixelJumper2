#pragma once

#include <random>

namespace Device
{
	namespace Random
	{
		void Initialize();
		std::mt19937& GetEngine();
		void FillRandom(
			const int size,
			char* const mem);

		template <typename T>
		T MakeRandom();

		template<>
		int Random::MakeRandom<int>();
		template<>
		double Random::MakeRandom<double>();
		template<>
		float Random::MakeRandom<float>();

		template <typename T>
		static T MakeRandom()
		{
			T value;

			FillRandom(sizeof(T), (char*)&value);

			return value;
		}
	}
}
