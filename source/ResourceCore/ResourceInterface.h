#pragma once

#include "ResourceBase.h"

#include <filesystem>
#include <string>

namespace Resource
{
	struct ResourceTypeDefinition
	{
		std::wstring name;
		std::wstring directory;
		std::wstring extension;
	};

	extern const ResourceTypeDefinition WorldResourceDefinition;
	extern const ResourceTypeDefinition SettingsResourceDefinition;
}

namespace Resource::Interface
{
	bool Initialize();

	/*
		Think about changing resourcetype to
		a resourcedefinition. so we can allow
		temporary resource types and do not 
		have to depent on some enum index
	*/

	bool SaveResource(
		std::wstring filename,
		// not const because save can
		// change internal values
		ResourceBase* resource,
		const ResourceTypeDefinition& type);
	bool LoadResource(
		std::wstring filename,
		ResourceBase* resource,
		const ResourceTypeDefinition& type);

	std::vector<char> LoadResourceRaw(
		std::wstring filename,
		const ResourceTypeDefinition& type);

	// let the user decide manually weather he wants
	// to cache a resource or not
	bool CacheResource(
		std::wstring filename,
		const ResourceTypeDefinition& type);

	inline bool LoadCacheResource(
		std::wstring filename,
		ResourceBase* resource,
		const ResourceTypeDefinition& type)
	{
		return CacheResource(filename, type)
			&& LoadResource(filename, resource, type);
	}

	std::filesystem::path GetResourcePath(
		std::wstring filename,
		const ResourceTypeDefinition& type);
}
