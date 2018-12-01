#include "SectionHost.h"

namespace Log
{
	SectionHost
		* SectionHost::root,
		* SectionHost::current;

	SectionHost::LoggingContext SectionHost::loggingContext;

	void SectionHost::onDelete() const
	{
		if (this == root)
		{
			logAllMessages();
		}
		else
		{
			if (parent->currentLevel < currentLevel)
			{
				parent->currentLevel = currentLevel;
			}

			current = parent;
		}
	}
	
	void SectionHost::logAllMessages() const
	{
		for (const Output& output : *Output::getOutputs())
		{
			loggingContext.currentOutput = &output;
			loggingContext.currentPrefix.clear();
			loggingContext.sectionIterator = sections.begin();

			logSectionBegin(currentLevel, name);

			loggingContext.currentPrefix = LOGGER_SECTION_SUBMESSAGE_PREFIX_END;

			logAllMessagesRecursive();
		}
	}
	
	void SectionHost::logAllMessagesRecursive() const
	{
		if (loggingContext.currentOutput->getImportance() > currentLevel)
		{
			return;
		}

		for (const _Log& log : logs)
		{
			logBasicPrint(&log);

			if (log.first == Level::Section)
			{
				extendPrefix();
				loggingContext.sectionIterator->logAllMessagesRecursive();
				reducePrefix();

				++loggingContext.sectionIterator;
			}
		}
	}
	
	void SectionHost::logBasicPrint(const _Log* const log) const
	{
		if (log->first == Level::Invalid)
		{
			logMessage(L"Fatal", L"Got invalid Log type");

			return;
		}

		if (log->first == Level::Section)
		{
			logSectionBegin(
				loggingContext.sectionIterator->currentLevel,
				loggingContext.sectionIterator->name
			);
		}
		else
		{
			logMessage(
				log->first,
				log->second
			);
		}
	}

	void SectionHost::logSectionBegin(
		const Level level, 
		const std::wstring name) const
	{
		std::wstring result = name + LOGGER_SECTION_SUFFIX;

		if (level == Level::Information)
		{
			result.append(LOGGER_SECTION_SUCCESS);
		}
		else
		{
			result.append(LOGGER_SECTION_FAILED);
		}

		logMessage(Level::Section, result);
	}
	
	void SectionHost::logMessage(
		const Level level, 
		const std::wstring message) const
	{
		switch (level)
		{
		case Level::Error:
			logMessage(LOGGER_ERROR_TITLE, message);

			break;
		case Level::Information:
			logMessage(LOGGER_INFORMATION_TITLE, message);

			break;
		case Level::Section:
			logMessage(LOGGER_SECTION_TITLE, message);

			break;
		case Level::Warning:
			logMessage(LOGGER_WARNING_TITLE, message);

			break;
		}
	}
	
	void SectionHost::logMessage(
		const std::wstring title, 
		const std::wstring message) const
	{
		std::wstring str;

		if (loggingContext.currentPrefix.size() > 0)
		{
			str = loggingContext.currentPrefix;
		}

		str.append(LOGGER_TITLE_BEGIN);
		str.append(title);
		str.append(LOGGER_TITLE_END);
		str.append(LOGGER_MESSAGE_PREFIX);
		str.append(message);
		str.append(L"\n");

		loggingContext.currentOutput->write(str);
	}
}
