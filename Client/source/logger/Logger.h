#pragma once

#include <Client/source/logger/Names.h>
#include <Client/source/logger/Output.h>
#include <Client/source/logger/SectionHost.h>
#include <Client/source/logger/Section.h>

#include <string>
#include <vector>

namespace Log
{
	namespace Independent
	{
		void Error(
			const std::wstring message);
		void Information(
			const std::wstring message);
		void Warning(
			const std::wstring message);
		void Message(
			const Level,
			const std::wstring title,
			const std::wstring message);
		void Message(
			const Output* const output,
			const std::wstring title,
			const std::wstring message);
		void Simple(
			const Output* const output,
			const std::wstring message);
	}

	void Error(
		const std::wstring message);
	void Information(
		const std::wstring message);
	void Warning(
		const std::wstring message);
}
