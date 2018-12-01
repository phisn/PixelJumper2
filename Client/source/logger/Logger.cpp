#include "Logger.h"

#include <Client/source/logger/Output.h>

#include <set>

namespace
{
	std::set<Logger::Output> outputs;
}

namespace Logger
{
	void Error(
		const std::wstring message)
	{
		SectionHost::getCurrent()->addLog(
			Level::Error,
			message);
	}
	
	void Information(
		const std::wstring message)
	{
		SectionHost::getCurrent()->addLog(
			Level::Information,
			message);
	}

	void Warning(
		const std::wstring message)
	{
		SectionHost::getCurrent()->addLog(
			Level::Warning,
			message);
	}
}
