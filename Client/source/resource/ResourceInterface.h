#pragma once

#include <Client/source/resource/ResourceBase.h>
#include <Client/source/resource/ResourceType.h>
#include <Client/source/resource/StaticResource.h>

#include <filesystem>

namespace Resource
{
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
