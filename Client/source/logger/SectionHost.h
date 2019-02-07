#pragma once

#include <Client/source/logger/LoggerLevel.h>
#include <Client/source/logger/Names.h>
#include <Client/source/logger/Output.h>

#include <string>
#include <vector>

namespace Log
{
	class SectionHost
	{
		friend class Section;
		typedef std::pair<Level, std::wstring> _Log;
	public:
		SectionHost(
			const std::wstring name)
			:
			SectionHost(name, NULL)
		{
		}

		SectionHost(
			const std::wstring name,
			SectionHost* const parent)
			:
			name(name),
			parent(parent)
		{
		}

		static SectionHost* Create(const std::wstring name);
		static SectionHost* getCurrent();

		void addLog(
			const Level level,
			const std::wstring message);

	private:
		static SectionHost
			* root,
			* current;

		void createSection(const std::wstring name);

		void onDelete() const;

		void logAllMessages() const;
		void logAllMessagesRecursive() const;

		void logBasicPrint(
			const _Log* const log) const;
		void logSectionBegin(
			const Level level,
			const std::wstring name) const;

		void logMessage(
			const Level level,
			const std::wstring message) const;
		void logMessage(
			const std::wstring title,
			const std::wstring message) const;

		void extendPrefix() const;
		void reducePrefix() const;

		Level currentLevel = Level::Information;
		const std::wstring name;
		
		std::vector<SectionHost> sections;
		std::vector<_Log> logs;

		mutable decltype(sections)::const_iterator sectionIterator;

		static struct LoggingContext
		{
			const Output* currentOutput;
			std::wstring currentPrefix;

		} loggingContext;

		SectionHost* const parent;
	};

	inline SectionHost* SectionHost::Create(
		const std::wstring name)
	{
		if (root)
		{
			current->createSection(name);
		}
		else
		{
			current = root = new SectionHost(name);
		}

		return current;
	}

	inline SectionHost* SectionHost::getCurrent()
	{
		return current;
	}

	inline void SectionHost::addLog(
		const Level level, 
		const std::wstring message)
	{
		if (level > currentLevel)
		{
			currentLevel = level;
		}

		logs.emplace_back(level, message);
	}

	inline void SectionHost::createSection(
		const std::wstring name)
	{
		// always in pair -> can be algo.
		// find out if log == recrusive sec.
		sections.emplace_back(name, this);
		logs.emplace_back(Level::Section, name);

		current = &sections.back();
	}

	inline void SectionHost::extendPrefix() const
	{
		loggingContext.currentPrefix =
			LOGGER_SECTION_SUBMESSAGE_PREFIX_MID
			+ loggingContext.currentPrefix;
	}

	inline void SectionHost::reducePrefix() const
	{
		loggingContext.currentPrefix = loggingContext.currentPrefix.substr(
			sizeof(LOGGER_SECTION_SUBMESSAGE_PREFIX_MID)
			/ sizeof(*LOGGER_SECTION_SUBMESSAGE_PREFIX_MID)
			- 1
		);
	}
}
