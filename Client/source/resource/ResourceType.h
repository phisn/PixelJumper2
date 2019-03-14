#pragma once

#include <SFML/Main.hpp>

namespace Resource
{
	enum class ResourceType
	{
		Settings,

		World,
		Player,
		
		_Length
	};

	namespace ResourceTypeMagic
	{
		typedef sf::Uint32 Type;

		static constexpr Type SETTINGS	= 0x0000'0000;

		static constexpr Type WORLD		= 0x0000'0000;
		static constexpr Type PLAYER	= 0x0000'0000;
	}

	namespace ResourceTypeName
	{
		static constexpr wchar_t SETTINGS[] = L"Dynamic";

		static constexpr wchar_t WORLD[] = L"World";
		static constexpr wchar_t PLAYER[] = L"Player";
	}

	namespace ResourceTypePath
	{
		static constexpr wchar_t SETTINGS[]	= L"settings";

		static constexpr wchar_t WORLD[]	= L"world";
		static constexpr wchar_t PLAYER[]	= L"player";
	}

	namespace ResourceTypeExtension
	{
		static constexpr bool HAS_SETTINGS = true;
		static constexpr wchar_t SETTINGS[] = L".stg";

		static constexpr bool HAS_WORLD = true;
		static constexpr wchar_t WORLD[] = L".wrld";

		static constexpr bool HAS_PLAYER = true;
		static constexpr wchar_t PLAYER[] = L".ply";
	}
}
