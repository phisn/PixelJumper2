#pragma once

#include <filesystem>
#include <map>
#include <set>
#include <string>

#define DEFUALT_RES_PATH L"resource/"
#define DEFAULT_RES_PATH_APPEND(resource) DEFUALT_RES_PATH #resource

namespace Resource
{
	typedef std::set<std::wstring> SubResources;
	typedef std::map<ResourceType, SubResources> MapedResources;

	enum class ResourceType
	{
		World,

		_Length
	};

	namespace Interface
	{
		bool Initialize();
		std::wstring Translate(const ResourceType resource);
	}
}
