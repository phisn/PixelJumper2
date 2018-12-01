#include "Output.h"

#include <fstream>
#include <iostream>

namespace Logger
{
	const Output::Out Output::FILE_OUT =
		[](const std::wstring message) -> void
	{
		std::wofstream file(
			path,
			std::ios::out | std::ios::app);

		if (file)
		{
			file.write(
				message.c_str(),
				message.size()
			);
		}
	};

	const Output::Out Output::CONSOLE_OUT =
		[](const std::wstring message) -> void
	{
		std::wcout << message;
	};

	std::wstring Output::path = L"logs.txt"; // default
}
