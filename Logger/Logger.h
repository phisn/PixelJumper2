#pragma once

// Main logger include file

#include "Names.h"
#include "Output.h"
#include "SectionHost.h"
#include "Section.h"

#include <codecvt>
#include <string>
#include <vector>

inline std::wstring carrtowstr(const char* const arr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	return converter.from_bytes(arr);
}

inline unsigned short bytetohex(const unsigned char byte)
{
	const char hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	unsigned short buffer;
	((char*) &buffer)[0] = hex[(byte & 0xF0) >> 4];
	((char*) &buffer)[1] = hex[(byte & 0x0F) >> 0];

	return buffer;
}

inline unsigned char hextobyte(unsigned short hex)
{
	const unsigned char left = ((char*) &hex)[0];
	const unsigned char right = ((char*) &hex)[1];

	unsigned char buffer = 0;

	buffer |= (left >= 'A') ? (left - 'A' + 10) : (left - '0');
	buffer <<= 4;
	buffer |= (right >= 'A') ? (right - 'A' + 10) : (right - '0');

	return buffer;
}

inline std::string carrtohexstr(const unsigned char* arr, size_t length = 0)
{
	if (length == 0)
	{
		length = strlen((const char*) arr);
	}

	std::string buffer;
	buffer.reserve(length * 2);

	for (int i = 0; i < length; ++i)
	{
		unsigned short hex = bytetohex(arr[i]);

		buffer += ((char*) &hex)[0];
		buffer += ((char*) &hex)[1];
	}

	return buffer;
}

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
	std::wstring Convert(const std::wstring message, Args... args);
	template <typename... Args>
	std::wstring Convert(const wchar_t* const message, Args... args);
	template <typename T, typename... Args>
	std::wstring Convert_Parameter(const T var, const std::wstring label, Args... args);
	template <typename T, typename... Args>
	std::wstring Convert_Parameter(const T var, const wchar_t* const label, Args... args);
	template <typename T>
	std::wstring Convert_Parameter(const T var, const std::wstring label);	
	template <typename T>
	std::wstring Convert_Parameter(const T var, const wchar_t* const label);

	template <typename... Args>
	std::wstring Convert(const std::wstring message, Args... args)
	{
		return message + L" " + Convert_Parameter(args...);
	}

	template <typename... Args>
	std::wstring Convert(const wchar_t* const message, Args... args)
	{
		return std::wstring(message) + L" " + Convert_Parameter(args...);
	}
	
	template <>
	inline std::wstring Convert<>(const std::wstring message)
	{
		return message;
	}

	template <>
	inline std::wstring Convert<>(const wchar_t* const message)
	{
		return std::wstring(message);
	}

	template <typename T, typename... Args>
	std::wstring Convert_Parameter(const T var, const std::wstring label, Args... args)
	{
		return label + L"=" + std::to_wstring(var) + L" " + Convert_Parameter(args...);
	}

	template <typename T, typename... Args>
	std::wstring Convert_Parameter(const T var, const wchar_t* const label, Args... args)
	{
		return std::wstring(label) + L"=" + std::to_wstring(var) + L" " + Convert_Parameter(args...);
	}

	template <typename T>
	std::wstring Convert_Parameter(const T var, const std::wstring label)
	{
		return label + L"=" + std::to_wstring(var) + L" ";
	}

	template <typename T>
	std::wstring Convert_Parameter(const T var, const wchar_t* const label)
	{
		return std::wstring(label) + L"=" + std::to_wstring(var) + L" ";
	}

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
