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

		enum Type
		{
			DefaultFont,

			_Length
		} type;
	}

	const int GetTranslationCount();
	const wchar_t** GetTranslations();

	const std::wstring Translate(
		Static::Type type);
}
