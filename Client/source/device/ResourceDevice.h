#pragma once

#include <Client/source/resource/ResourceManager.h>

#include <SFML/System/MemoryInputStream.hpp>

#include <fstream>
#include <map>

#ifdef _WIN32
#include <Windows.h>

#define _EXISTS(file) !(INVALID_FILE_ATTRIBUTES == GetFileAttributesW(file) \
	&& GetLastError() == ERROR_FILE_NOT_FOUND)
#endif

namespace Resource
{
	enum class File
	{
		DefaultFont
	};
}

namespace Device
{
	/*
	
		Handling resources and settings
		using files
	
	*/

	class Resource
	{
	public:
		bool initialize();

		sf::MemoryInputStream load(
			const ::Resource::File resource);
	};
}
