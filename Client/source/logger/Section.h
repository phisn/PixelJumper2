#pragma once

#include <Client/source/logger/Level.h>
#include <Client/source/logger/Logger.h>

namespace Logger
{
	class Section // SectionHost wrapper for RAII
	{
	public:
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
			Error(message);
		}

		void information(
			const std::wstring message)
		{
			Information(message);
		}

		void warning(
			const std::wstring message)
		{
			Warning(message);
		}

	private:
		SectionHost* const host;
	};
}
