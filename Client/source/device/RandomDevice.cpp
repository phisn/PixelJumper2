#include "RandomDevice.h"

namespace
{
	std::mt19937 engine;
	std::uniform_int_distribution<std::mt19937::result_type> charDistribution(0x00, 0xff);
}

namespace Device
{
	void Random::Initialize()
	{
		engine.seed(std::random_device()());
	}

	std::mt19937& Random::GetEngine()
	{
		return engine;
	}

	void Random::FillRandom(
		const int size, 
		char* const mem)
	{
		for (int i = 0; i < size; ++i)
			mem[i] = charDistribution(engine);
	}

	template<>
	int Random::MakeRandom<int>()
	{
		static std::uniform_int_distribution<int> distribution(INT_MIN, INT_MAX);
		return distribution(GetEngine());
	}

	template<>
	long long Random::MakeRandom<long long>()
	{
		static std::uniform_int_distribution<long long> distribution(INT_MIN, INT_MAX);
		return distribution(GetEngine());
	}

	template<>
	double Random::MakeRandom<double>()
	{
		static std::uniform_real_distribution<double> distribution(DBL_MIN, DBL_MAX);
		return distribution(GetEngine());
	}

	template<>
	float Random::MakeRandom<float>()
	{
		static std::uniform_real_distribution<float> distribution(FLT_MIN, FLT_MAX);
		return distribution(GetEngine());
	}
}
