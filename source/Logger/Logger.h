#pragma once

// Main logger include file

#include "Conversion.h"
#include "Names.h"
#include "Output.h"
#include "SectionHost.h"
#include "Section.h"

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
	

		Update:
		-	Logs now allow paramters handled
		by the convert function. Old styles are
		still supported
		-	Normal wstring can be used as paramters
		and will be concated with spaces
		-	All other types need a string label and
		will be converted with std::to_wstring
		-	Ex: Information(L"Hello", 5, L"important", L"World", 'T', L"someChar");
			Rs: [Information] : Hello important=5 World someChar=T
	*/

	void Error(const std::wstring message);
	void Information(const std::wstring message);
	void Warning(const std::wstring message);

	namespace Independent
	{
		void Error(const std::wstring message);
		void Information(const std::wstring message);
		void Warning(const std::wstring message);

		void Simple(
			const Output* const output,
			const std::wstring message);
		void Message(
			const Level,
			const std::wstring title,
			const std::wstring message);
		void Message(
			const Output* const output,
			const std::wstring title,
			const std::wstring message);
	}

	// wstring(message)
	// T(value) [to_wstring] "=" wstring(label)

	template <typename... Args>
	void Error(Args... args)
	{
		Error(Convert(args...));
	}

	template <typename... Args>
	void Information(Args... args)
	{
		Information(Convert(args...));
	}

	template <typename... Args>
	void Warning(Args... args)
	{
		Warning(Convert(args...));
	}
}
