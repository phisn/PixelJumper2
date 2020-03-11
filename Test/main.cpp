#include "Common/RandomModule.h"
#include <iostream>

int main()
{
	char buffer[200];
	
	Module::Random::FillRandom(
		200,
		(char*)buffer
	);
}