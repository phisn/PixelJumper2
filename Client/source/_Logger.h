#pragma once

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#define LOGGER_ERROR_TITLE L"Error"
#define LOGGER_INFORMATION_TITLE L"Information"
#define LOGGER_WARNING_TITLE L"Warning"

#define LOGGER_TITLE_BEGIN L"["
#define LOGGER_TITLE_END L"]"
#define LOGGER_MESSAGE_PREFIX L" : "

#define LOGGER_SECTION_FAILED L"Failed"
#define LOGGER_SECTION_SUCCESS L"Success"
#define LOGGER_SECTION_TITLE L"Section"

#define LOGGER_SECTION_MESSAGE_PREFIX L"-> "
#define LOGGER_SECTION_PREFIX L""
#define LOGGER_SECTION_SUFFIX L" ... "

namespace Log
{
	typedef void (*Output)(const std::wstring);

	extern const Output OUTPUT_CONSOLE;
	extern const Output OUTPUT_FILE;

	void AddOutput(
		Output output);
	void RemoveOutput(
		Output output);

	void SetOutputFilePath(
		const std::wstring path);

	namespace Independent
	{
		void Error(
			const std::wstring message);
		void Information(
			const std::wstring message);
		void Warning(
			const std::wstring message);

		void Message(
			const std::wstring title,
			const std::wstring message,
			const bool appendLine = true);
		void Output(
			const std::wstring message);
	}

	void Error(
		const std::wstring message);
	void Information(
		const std::wstring message);
	void Warning(
		const std::wstring message);

	class Section
	{
	public:
		enum class Level
		{
			Invalid,
			Error,
			Information,
			Warning
		};

		// level indicates  the point from
		// on all logged  messages will be
		// printed like shown above (error)
		Section(
			const std::wstring title,
			const Level level = Level::Error)
			:
			trigger(level)
		{
			Log::Independent::Message(
				LOGGER_SECTION_TITLE,
				LOGGER_SECTION_PREFIX
				+ title
				+ LOGGER_SECTION_SUFFIX, false
			);
		}

		~Section();

		void error(
			const std::wstring message);
		void information(
			const std::wstring message);
		void warning(
			const std::wstring message);

		static Section* getCurrent()
		{
			return current;
		}
	private:
		static Section* current;

		void updateHighest(
			const Level level);

		std::vector<
			std::pair<Level, std::wstring>
		> logs;

		Level trigger, highest = Level::Invalid;
		std::wstring name;
	};

	inline void Section::error(
		const std::wstring message)
	{
		updateHighest(Level::Error);

		logs.emplace_back(
			Level::Error,
			message);
	}

	inline void Section::information(
		const std::wstring message)
	{
		updateHighest(Level::Information);

		logs.emplace_back(
			Level::Information,
			message);
	}

	inline void Section::warning(
		const std::wstring message)
	{
		updateHighest(Level::Warning);

		logs.emplace_back(
			Level::Warning,
			message);
	}

	inline void Section::updateHighest(
		const Section::Level level)
	{
		if ((int)level > (int)highest)
			highest = level;
	}
}
