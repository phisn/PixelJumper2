#pragma once

// Main logger include file

#include <Client/source/logger/Names.h>
#include <Client/source/logger/Output.h>
#include <Client/source/logger/SectionHost.h>
#include <Client/source/logger/Section.h>

#include <string>
#include <vector>

namespace Log
{
	/*
	
		Usage:
		-	Use normal Error_Information_Warning
		functions for quick and easy logging.
		-	Use independent function to force
		logging and be independent from upper sections
		-	Use section to group complex processes
		which are completed in short time. should not
		be used in long loops / long functions
		-	Logging functions can be used though
		the object itself. Normal EIW functions
		(indirectly a alias) can also be used instead
	
	*/

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
