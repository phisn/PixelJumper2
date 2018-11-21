#pragma once

#include <string>

namespace Resource
{
	enum class File
	{
		DefaultFont,

		_Size
	};

	namespace Manager
	{
		std::wstring translate(const File resource);
	}
}