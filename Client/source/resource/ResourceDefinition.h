#pragma once

#include <Client/source/resource/ResourceBase.h>
#include <Client/source/resource/ResourceType.h>

namespace Resource
{
	struct ResourceDefinition
	{
		static const ResourceDefinition* Get(const ResourceType type);

		ResourceDefinition(
			const sf::Uint32 magic,
			const bool hasExtension,
			const wchar_t* const extension,
			const wchar_t* const name,
			const wchar_t* const path)
			:
			magic(magic),
			hasExtension(hasExtension),
			extension(extension),
			name(name),
			path(path)
		{
		}

		const ResourceTypeMagic::Type magic;

		const bool hasExtension;
		const wchar_t* const extension;

		const wchar_t* const name;
		const wchar_t* const path;
	};
}
