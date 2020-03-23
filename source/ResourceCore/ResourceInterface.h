#pragma once

#include "ResourceBase.h"
#include "ResourceType.h"
#include "StaticResource.h"

#include <filesystem>

namespace Resource
{
	/*
	
		ideas:
		-	iterate though all resources of a kind
		-	filter and search resources of a kind or all
		-	load readonly resource to allow caching resources
			and automatic "garbage collection"
	
	*/
	namespace Interface
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
