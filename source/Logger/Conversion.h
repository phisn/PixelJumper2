#pragma once

#include <codecvt>
#include <filesystem>
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
	((char*)&buffer)[0] = hex[(byte & 0xF0) >> 4];
	((char*)&buffer)[1] = hex[(byte & 0x0F) >> 0];

	return buffer;
}

inline unsigned char hextobyte(unsigned short hex)
{
	const unsigned char left = ((char*)&hex)[0];
	const unsigned char right = ((char*)&hex)[1];

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
		length = strlen((const char*)arr);
	}

	std::string buffer;
	buffer.reserve(length * 2);

	for (int i = 0; i < length; ++i)
	{
		unsigned short hex = bytetohex(arr[i]);

		buffer += ((char*)&hex)[0];
		buffer += ((char*)&hex)[1];
	}

	return buffer;
}


template <typename T>
inline std::wstring convert_to_wstring(T value)
{
	if constexpr (std::is_enum_v<T>)
	{
		return std::to_wstring((long long)value);
	}
	else
	{
		return std::to_wstring(value);
	}
}

template <>
inline std::wstring convert_to_wstring<std::wstring>(const std::wstring value)
{
	return L'\'' + value + L'\'';
}

template <>
inline std::wstring convert_to_wstring<const wchar_t*>(const wchar_t* value)
{
	return L'\'' + std::wstring(value) + L'\'';
}

template <>
inline std::wstring convert_to_wstring<std::string>(const std::string value)
{
	return L'\'' + carrtowstr(value.c_str()) + L'\'';
}

template <>
inline std::wstring convert_to_wstring<const char*>(const char* value)
{
	return L'\'' + carrtowstr(value) + L'\'';
}

template <>
inline std::wstring convert_to_wstring<std::filesystem::path>(std::filesystem::path value)
{
	return value.wstring();
}

template <typename T>
inline std::wstring convert_to_wstring(T* ptr)
{
	// [int*] 0x2955712398
	return L"(" + carrtowstr(typeid(T).name()) + L"*) 0x" + convert_to_wstring((std::ptrdiff_t) ptr);
}

namespace Log
{
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
		return label + L"=" + convert_to_wstring(var) + L" " + Convert_Parameter(args...);
	}

	template <typename T, typename... Args>
	std::wstring Convert_Parameter(const T var, const wchar_t* const label, Args... args)
	{
		return std::wstring(label) + L"=" + convert_to_wstring(var) + L" " + Convert_Parameter(args...);
	}

	template <typename T>
	std::wstring Convert_Parameter(const T var, const std::wstring label)
	{
		return label + L"=" + convert_to_wstring(var) + L" ";
	}

	template <typename T>
	std::wstring Convert_Parameter(const T var, const wchar_t* const label)
	{
		return std::wstring(label) + L"=" + convert_to_wstring(var) + L" ";
	}
}
