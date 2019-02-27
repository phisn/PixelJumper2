#pragma once

#include <SFML/Main.hpp>

#include <string>
#include <vector>

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

		std::vector<std::wstring> GetTranslations();

		std::wstring GetFolderName();
		std::wstring GetPath();

		std::wstring Translate(const Static::ID id);
	}
}
