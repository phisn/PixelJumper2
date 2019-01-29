#pragma once

#include <Client/source/logger/LoggerLevel.h>
#include <Client/source/logger/SectionHost.h>

namespace Log
{
	class Section // SectionHost wrapper for RAII
	{
	public:
		Section(
			const std::wstring name)
			:
			Section( SectionHost::Create(name) )
		{
		}

		Section(
			SectionHost* const host)
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
