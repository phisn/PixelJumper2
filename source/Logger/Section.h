#pragma once

#include "Conversion.h"
#include "LoggerLevel.h"
#include "SectionHost.h"

namespace Log
{
	class Section // SectionHost wrapper for RAII
	{
	public:
		template <typename... Args>
		Section(std::wstring name, Args... args)
			:
			Section(SectionHost::Create(Convert(name, args...)))
		{
		}

		Section(SectionHost* const host)
			:
			host(host)
		{
		}

		~Section()
		{
			host->onDelete();
		}

		void error(
			const std::wstring message)
		{
			host->addLog(Level::Error, message);
		}

		void information(
			const std::wstring message)
		{
			host->addLog(Level::Information, message);
		}

		void warning(
			const std::wstring message)
		{
			host->addLog(Level::Warning, message);
		}

	private:
		SectionHost* const host;
	};
}
