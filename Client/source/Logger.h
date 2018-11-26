#pragma once

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#define LOGGER_ERROR_TITLE L"Error"
#define LOGGER_INFORMATION_TITLE L"Information"
#define LOGGER_WARNING_TITLE L"Warning"

#define LOGGER_SECTION_FAILED L"Failed"
#define LOGGER_SECTION_SUCCESS L"Success"
#define LOGGER_SECTION_TITLE L"Section"

#define LOGGER_SECTION_PREFIX L""
#define LOGGER_SECTION_SUFFIX L"..."

namespace Log
{
	/*

		// use RAII
	
		Section section = Log::EnterSection(L"Initializing Resource");
		section.inform(L"Received 10213");

		if (abc) {
			section.error(L"Unable to abc");
			return;
		}

		//

		abc == true:

		[Section] : Initializing Screen ... Success
		[Section] : Initializing Resource ... Failed
		-> [Information] : Received 10213
		-> [Error]
		[Information] : Retrying ...

	*/

	typedef void(*Output)(const std::wstring);

	extern const Output OUTPUT_CONSOLE;
	extern const Output OUTPUT_FILE;

	void AddOutput(
		Output output);
	void RemoveOutput(
		Output output);
	
	class Section;

	// only called by section itself
	void SectionEnter();
	void SectionLeave(
		const bool success);

	void Error(
		const std::wstring message, 
		const bool appendLine = true);
	void Information(
		const std::wstring message,
		const bool appendLine = true);
	void Warning(
		const std::wstring message,
		const bool appendLine = true);
	void Message(
		const std::wstring title,
		const std::wstring message,
		const bool appendLine = true);
	void OutputRaw(
		const std::wstring raw);

	void SetOutputFilePath(
		const std::wstring path);


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

		~Section()
		{
			Log::LeaveSection(highest != Level::Error);

			if ((int)highest >= (int)trigger)
				for (std::pair<Level, std::wstring>& log : logs)
				{
					Log::OutputRaw(L"-> ");

					switch (log.first)
					{
					case Level::Error:
						Log::Error(log.second);

						break;
					case Level::Information:
						Log::Information(log.second);

						break;
					case Level::Warning:
						Log::Warning(log.second);

						break;
					}
				}
		}

		// level indicates  the point from
		// on all logged  messages will be
		// printed like shown above (error)
		Section(
			const std::wstring title,
			const Level level = Level::Error)
			:
			trigger(level)
		{
			Log::Message(
				LOGGER_SECTION_TITLE,
				LOGGER_SECTION_PREFIX
				+ title
				+ LOGGER_SECTION_SUFFIX
			);
		}

		void error(
			const std::wstring message)
		{
			updateHighest(Level::Error);

			logs.emplace_back(
				Level::Error,
				message);
		}

		void information(
			const std::wstring message)
		{
			updateHighest(Level::Information);

			logs.emplace_back(
				Level::Information,
				message);
		}

		void warning(
			const std::wstring message)
		{
			updateHighest(Level::Warning);

			logs.emplace_back(
				Level::Warning,
				message);
		}

	private:
		void updateHighest(
			const Level level)
		{
			if ((int)level > (int)highest)
				highest = level;
		}

		std::vector<
			std::pair<Level, std::wstring>
		> logs;

		Level trigger, highest = Level::Invalid;
		std::wstring name;
	};
}
