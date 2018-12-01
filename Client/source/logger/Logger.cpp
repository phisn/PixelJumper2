#include "Logger.h"


#include <set>

namespace
{
	std::set<Log::Output> outputs;
}

namespace Log
{
	void Independent::Error(
		const std::wstring message)
	{
		Message(Level::Warning, LOGGER_ERROR_TITLE, message);
	}

	void Independent::Information(
		const std::wstring message)
	{
		Message(Level::Warning, LOGGER_INFORMATION_TITLE, message);
	}

	void Independent::Warning(
		const std::wstring message)
	{
		Message(Level::Warning, LOGGER_WARNING_TITLE, message);
	}

	void Independent::Message(
		const Level level, 
		const std::wstring title, 
		const std::wstring message)
	{
		for (const Output& output : *Output::getOutputs())
			if (output.getImportance() <= level)
			{
				Message(&output, title, message);
			}
	}

	void Independent::Message(
		const Output* const output,
		const std::wstring title, 
		const std::wstring message)
	{
		std::wstring str = LOGGER_TITLE_BEGIN;

		str.append(title);
		str.append(LOGGER_TITLE_END);
		str.append(LOGGER_MESSAGE_PREFIX);
		str.append(message);
		str.append(L"\n");

		Simple(output, str);
	}

	void Independent::Simple(
		const Output* const output,
		const std::wstring message)
	{
		output->write(message);
	}

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
