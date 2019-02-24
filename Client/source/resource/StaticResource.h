#pragma once

#include <SFML/Main.hpp>

#include <string>
#include <utility>

#ifndef RESOURCE
#define RESOURCE ::Resource
#endif

namespace Resource
{
	namespace Static
	{
		typedef std::pair<char*, sf::Int64> Resource;

		enum ID
		{
			DefaultFont,

			_Length
		};

		const int GetTranslationCount();
		const wchar_t** GetTranslations();

		std::wstring Translate(const Static::ID id);
	}
}
