#pragma once

#include <Client/source/logger/Level.h>
#include <Client/source/logger/Output.h>
#include <Client/source/logger/Section.h>

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

#define LOGGER_SECTION_SUBMESSAGE_PREFIX_END L"->"
#define LOGGER_SECTION_SUBMESSAGE_PREFIX_MID L"--"

#define LOGGER_SECTION_PREFIX L""
#define LOGGER_SECTION_SUFFIX L" ... "

namespace Logger
{
	namespace Independent
	{

	}

	void Error(
		const std::wstring message);
	void Information(
		const std::wstring message);
	void Warning(
		const std::wstring message);

	class SectionHost
	{
		typedef std::pair<Level, std::wstring> Log;
	public:
		static SectionHost* Create(
			const std::wstring name)
		{
			if (root)
			{
				current->addSection(name);
			}
			else
			{
				current = root = new SectionHost(name);
			}
		}

		static SectionHost* getCurrent()
		{
			return current;
		}

		void onDelete()
		{
			if (this == root)
			{
#ifdef _DEBUG
				if (current != root)
				{
					Output::getOutputs()[0][0].write(L"LOG WRONGLY DELETED");
				}
#endif

				logMessages();

				delete root;
				root = NULL;
			}
			else
			{
				current = parent;
			}
		}

		void addSection(
			const std::wstring name)
		{
			logs.emplace_back(Level::Section, name);
			subSections.emplace_back(this);
		}

		void addLog(
			const Level level,
			const std::wstring message)
		{
			if (level > this->level)
			{
				this->level = level;
			}

			logs.emplace_back(level, message);
		}

	private:
		static SectionHost
			* root,
			* current;

		SectionHost(
			const std::wstring name)
			:
			SectionHost(NULL)
		{
			// rethink
			logSectionEntry(&name);
		}

		SectionHost(
			SectionHost* const parent)
			:
			parent(parent)
		{
		}

		void logMessages() 
		{
			for (const Output& output : *Output::getOutputs())
			{
				logMessages(&output);
			}
		}

		void logMessages(
			const Output* const output)
		{
			std::wstring prefix = LOGGER_SECTION_SUBMESSAGE_PREFIX_END;
			subSecIt = subSections.begin();

			logSectionStatus(output, this->level);
			logSection(output, &prefix);
		}

		void logSection(
			const Output* const output,
			std::wstring* const prefix) const
		{
			for (const Log& log : logs)
			{
				logBasic(output, &log);

				if (log.first == Level::Section)
				{
					if (subSecIt == subSections.cend())
					{
#ifdef _DEBUG
						Output::getOutputs()[0][0].write(L"INVALID SUBSECTION");
#endif

						continue;
					}

					if (subSecIt->level >= output->getImportance())
					{
						continue;
					}

					*prefix = LOGGER_SECTION_SUBMESSAGE_PREFIX_MID + *prefix;
					
					subSecIt->logSection(output, prefix);
					
					prefix->substr(
						sizeof(LOGGER_SECTION_SUBMESSAGE_PREFIX_MID) 
						/ sizeof(*LOGGER_SECTION_SUBMESSAGE_PREFIX_MID) - 1
					);
					
					++subSecIt;
				}

			}
		}

		void logBasic(
			const Output* const output,
			const Log* const log) const
		{
			if (log->first == Level::Invalid)
			{
#ifdef _DEBUG
				Output::getOutputs()[0][0].write(L"INVALID LEVEL");
#endif
			}

			if (log->first == Level::Section)
			{
				logSectionEntry(
					output, 
					log->second, 
					subSecIt->level
				);
			}
			else
			{
				logMessage(
					output, 
					log->first, 
					log->second
				);
			}
		}

		void logSectionEntry(
			const Output* output,
			const std::wstring const name,
			const Level level) const
		{
			logMessage(output, Level::Section, name);

			if (level != Level::Invalid)
			{
				logSectionStatus(output, level);
			}
		}

		void logSectionStatus(
			const Output* output,
			const Level level) const
		{
			const std::wstring message = level == Level::Information
				? LOGGER_SECTION_SUCCESS
				: LOGGER_SECTION_FAILED;

			output->write(message);
		}

		void logMessage(
			const Output* const output,
			const Level level,
			const std::wstring message) const
		{
			std::wstring str = LOGGER_TITLE_BEGIN;

			switch (level)
			{
			case Level::Error:
				str.append(LOGGER_ERROR_TITLE);

				break;
			case Level::Information:
				str.append(LOGGER_INFORMATION_TITLE);

				break;
			case Level::Section:
				str.append(LOGGER_SECTION_TITLE);

				break;
			case Level::Warning:
				str.append(LOGGER_WARNING_TITLE);

				break;
			}

			str.append(LOGGER_TITLE_END);
			str.append(LOGGER_MESSAGE_PREFIX);
			str.append(message);

			output->write(str);
		}

		Level level;

		std::vector<SectionHost> subSections;
		std::vector<Log> logs;

		mutable decltype(subSections)::iterator subSecIt;

		SectionHost* const parent;
	};
}
