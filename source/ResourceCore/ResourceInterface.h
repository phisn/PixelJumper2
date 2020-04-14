#pragma once

#include "ResourceBase.h"
#include "ResourceType.h"
#include "StaticResource.h"

#include "WorldResource.h"

#include <filesystem>

namespace Resource
{
	struct ResourceTypeDefinition
	{
		std::string name;
		std::string directory;
		std::string extension;
	};

	const ResourceTypeDefinition WorldResourceDefinition;
}

namespace Resource::Interface
{
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

	// let the user decide manually weather he wants
	// to cache a resource or not
	bool CacheResource(
		std::wstring filename,
		const ResourceTypeDefinition& type);

	bool LoadCacheResource(
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

namespace Resource
{
	/*
	
		ideas:
		-	iterate though all resources of a kind
		-	filter and search resources of a kind or all
		-	load readonly resource to allow caching resources
			and automatic "garbage collection"
	
	*/
	namespace _Interface
	{
		bool Initialize();
		void Uninitalize();

		bool MapResources();
		bool MapResourceType(const ResourceType type);

		bool SaveResource(
			ResourceBase* const resource,
			const ResourceType type,
			const std::wstring name);
		bool LoadResource(
			ResourceBase* const resource,
			const ResourceType type,
			const std::wstring name);

		bool SaveSettings(
			ResourceBase* const resource,
			const std::wstring name);
		bool LoadSettings(
			ResourceBase* const resource,
			const std::wstring name);

		Static::Resource GetStaticResource(
			const Static::Id type);
		Static::Resource GetStaticResource(
			const std::filesystem::path resource);

		const std::wstring GetResourcePath();
		const std::wstring MakeResourceTypePath(const ResourceType type);
		const std::wstring MakeResourceFilePath(
			const ResourceType type,
			const std::wstring name);
	}
}
