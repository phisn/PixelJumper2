#pragma once

#include <Client/source/logger/Logger.h>

#include <Client/source/resource/pipes/FilePipe.h>

#include <Client/source/resource/Common.h>
#include <Client/source/resource/ResourceBase.h>
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

		Static::Resource GetStaticResource(
			const Static::Type type);
		Static::Resource GetStaticResource(
			const std::filesystem::path resource);

		const ResourceDefinition* GetDefinition(const ResourceType resource);
		std::wstring Translate(const ResourceType resource);
	}
}
