#pragma once

#include <Client/source/logger/Logger.h>
#include <Client/source/resource/Common.h>
#include <Client/source/resource/ResourceBase.h>

#include <SFML/Main.hpp>

#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <string>

namespace Resource
{
	struct MappedResource
	{
		const std::wstring path;
	};

	typedef std::map<std::wstring, MappedResource> SubResources; // name, data
	typedef std::map<ResourceType, SubResources> MapedResources; // type, list

	namespace Interface
	{
		bool Initialize();

		bool RemapAllFiles();
		bool RemapFiles(const ResourceType type);

		bool SaveResource(
			const Base* resource,
			const ResourceType type,
			const std::wstring name);
		Base* AllocateResource(
			const ResourceType type, 
			const std::wstring name);

		const Definition* GetDefinition(const ResourceType resource);
		std::wstring Translate(const ResourceType resource);
	}
}
