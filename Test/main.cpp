#include "Common/RandomModule.h"
#include "Logger/Logger.h"
#include <iostream>

class Test
{

};
int main()
{

	Test t1;
	Test* t2 = &t1;

	Log::Output::Add(Log::Output::CONSOLE_OUT, Log::Level::Information);
	Log::Information(L"info", t2, L"ptr");
}