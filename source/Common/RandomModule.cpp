#include "RandomModule.h"

namespace
{
	std::mt19937 engine;
	std::uniform_int_distribution<std::mt19937::result_type> charDistribution(0x00, 0xff);
}

namespace Module
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
		{
			mem[i] = MakeRandom<signed char>();
		}
	}
}
