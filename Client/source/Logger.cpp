#include "Logger.h"

#include <set>
#include <stack>

namespace
{
	std::set<Log::Output> outputs;
	std::wstring path;
}

namespace Log
{
	const Output OUTPUT_CONSOLE = 
		[](const std::wstring message) -> void
	{
		std::wcout << message;
	};

	const Output OUTPUT_FILE = 
		[](const std::wstring message) -> void
	{
		std::wofstream file(
			path,
			std::ios::out | std::ios::app);

		if (file)
		{
			file.write(
				message.c_str(),
				message.size()
			);
		}
	};

	void AddOutput(
		Output output)
	{
		outputs.insert(output);
	}

	void RemoveOutput(
		Output output)
	{
		outputs.erase(output);
	}

	void Error(
		const std::wstring message,
		const bool appendLine)
	{
		Message(LOGGER_ERROR_TITLE, message, appendLine);
	}

	void Information(
		const std::wstring message,
		const bool appendLine)
	{
		Message(LOGGER_INFORMATION_TITLE, message, appendLine);
	}
	
	void Warning(
		const std::wstring message,
		const bool appendLine)
	{
		Message(LOGGER_WARNING_TITLE, message, appendLine);
	}

	void Message(
		const std::wstring title, 
		const std::wstring message,
		const bool appendLine)
	{
		const std::wstring completeMessage = L"[" + title + L"] -> " + message + L"\n";

		for (const Output& output : outputs)
		{
			output(completeMessage);
		}
	}

	void SetOutputFilePath(
		const std::wstring path)
	{
		::path = path;
	}
}
