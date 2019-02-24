#pragma once

#include <SFML/Main.hpp>

namespace Resource
{
	enum class ResourceType
	{
		Static,
		Dynamic,

		World,
		Player,
		
		_Length
	};

	namespace ResourceTypeMagic
	{
		static constexpr sf::Uint32 STATIC	= 0x0000'0000;
		static constexpr sf::Uint32 DYNAMIC	= 0x0000'0000;

		static constexpr sf::Uint32 WORLD	= 0x0000'0000;
		static constexpr sf::Uint32 PLAYER	= 0x0000'0000;
	}

	namespace ResourceTypeName
	{
		static constexpr wchar_t STATIC[] = L"Static";
		static constexpr wchar_t DYNAMIC[] = L"Dynamic";

		static constexpr wchar_t WORLD[] = L"World";
		static constexpr wchar_t PLAYER[] = L"Player";
	}

	namespace ResourceTypePath
	{
		static constexpr wchar_t STATIC[]	= L"assests";
		static constexpr wchar_t DYNAMIC[]	= L"settings";

		static constexpr wchar_t WORLD[]	= L"world";
		static constexpr wchar_t PLAYER[]	= L"player";
	}

	namespace ResourceTypeExtension
	{
		static constexpr bool HAS_STATIC = false;
		static constexpr bool HAS_DYNAMIC = true;

		static constexpr bool HAS_WORLD = true;
		static constexpr bool HAS_PLAYER = true;

		static constexpr wchar_t STATIC[] = L"";
		static constexpr wchar_t DYNAMIC[] = L"stg";

		static constexpr wchar_t WORLD[] = L"wrld";
		static constexpr wchar_t PLAYER[] = L"ply";
	}
}
