#pragma once

#include <Client/source/resource/ResourceBase.h>
#include <Client/source/resource/ResourceType.h>

namespace Resource
{
	struct Definition
	{
		static const Definition* Get(const ResourceType type);

		Definition(
			const sf::Uint32 magic,
			const bool hasExtension,
			const wchar_t* const extension,
			const wchar_t* const name,
			const wchar_t* const path)
			:
			magic(magic),
			name(name),
			path(path)
		{
		}

		const sf::Uint32 magic;

		const bool hasExtension;
		const wchar_t* const extension;

		const wchar_t* const name;
		const wchar_t* const path;
	};
}
