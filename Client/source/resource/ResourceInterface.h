#pragma once

#include <Client/source/logger/Logger.h>

#include <Client/source/resource/pipes/FilePipe.h>

#include <Client/source/resource/ResourceBase.h>
#include <Client/source/resource/ResourceType.h>
#include <Client/source/resource/StaticResource.h>

#include <SFML/Main.hpp>

#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <string>

namespace Resource
{
	typedef std::map<std::wstring, FileDefinition> SubResources; // name, data
	typedef std::map<ResourceType, SubResources> MapedResources; // type, list

	namespace Interface
	{
		bool Initialize();

		bool RemapAllFiles();
		bool RemapFiles(const ResourceType type);

		bool WriteResource(
			ResourceBase* const resource,
			const ResourceType type,
			const std::wstring name);
		bool ReadResource(
			ResourceBase* const resource,
			const ResourceType type, 
			const std::wstring name);

		bool ReadRawResource(
			ResourceBase* const resource,
			const std::filesystem::path path);
		bool ReadRawResource(
			ResourceBase* const resource,
			const FileDefinition* const file);
		bool WriteRawResource(
			ResourceBase* const resource,
			const std::filesystem::path path);
		bool WriteRawResource(
			ResourceBase* const resource,
			FileDefinition* const file);
		
		Static::Resource GetStaticResource(
			const Static::ID type);
		Static::Resource GetStaticResource(
			const std::filesystem::path resource);

		const std::wstring GetResourcePath();
		const std::wstring MakeResourcePath(const ResourceType type);
	}
}
