#include "Logger.h"

#include <set>
#include <stack>
#include "logger\Logger.h"

namespace
{
	std::set<Log::Output> outputs;
	std::wstring path;
}

namespace Log
{
	Section* Section::current = NULL;

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

	void SetOutputFilePath(
		const std::wstring path)
	{
		::path = path;
	}

	void Log::Independent::Error(
		const std::wstring message)
	{
		Message(LOGGER_ERROR_TITLE, message);
	}

	void Log::Independent::Information(
		const std::wstring message)
	{
		Message(LOGGER_INFORMATION_TITLE, message);
	}

	void Log::Independent::Warning(
		const std::wstring message)
	{
		Message(LOGGER_WARNING_TITLE, message);
	}

	void Log::Independent::Message(
		const std::wstring title,
		const std::wstring message,
		const bool appendLine)
	{
		std::wstring str;

		str.append(LOGGER_TITLE_BEGIN);
		str.append(title);
		str.append(LOGGER_TITLE_END);
		
		str.append(LOGGER_MESSAGE_PREFIX);
		str.append(message);

		if (appendLine)
		{
			str.append(L"\r\n");
		}

		Output(str);
	}

	void Log::Independent::Output(
		const std::wstring message)
	{
		for (const Log::Output& output : outputs)
		{
			output(message);
		}
	}

	void Error(
		const std::wstring message)
	{
		Section* const section = Section::getCurrent();

		if (section)
		{
			section->error(message);
		}
		else
		{
			Independent::Error(message);
		}
	}

	void Information(
		const std::wstring message)
	{
		Section* const section = Section::getCurrent();

		if (section)
		{
			section->information(message);
		}
		else
		{
			Independent::Information(message);
		}
	}
	
	void Warning(
		const std::wstring message)
	{
		Section* const section = Section::getCurrent();

		if (section)
		{
			section->warning(message);
		}
		else
		{
			Independent::Warning(message);
		}
	}

	void Message(
		const std::wstring title, 
		const std::wstring message)
	{
		Independent::Message(title, message);
	}

	Section::~Section()
	{
		current = NULL;

		if ((int)highest >= (int)trigger)
		{
			Log::Independent::Output(L"Failed\r\n");

			for (std::pair<Level, std::wstring>& log : logs)
			{
				Log::Independent::Output(LOGGER_SECTION_MESSAGE_PREFIX);

				switch (log.first)
				{
				case Level::Error:
					Log::Independent::Error(log.second);

					break;
				case Level::Information:
					Log::Independent::Information(log.second);

					break;
				case Level::Warning:
					Log::Independent::Warning(log.second);

					break;
				}
			}
		}
		else
		{
			Log::Independent::Output(L"Success\r\n");
		}
	}
}
