#include "ResourceInterface.h"

namespace
{
	Resource::MapedResources resources;

	const wchar_t* resourceTypes[(int) Resource::ResourceType::_Length]
	{
		DEFAULT_RES_PATH_APPEND(L"world/")
	};
}

namespace Resource
{
	bool Interface::Initialize()
	{
		// iterate DEFAULT_RES_PATH
	}

	std::wstring Interface::Translate(
		const ResourceType resource)
	{
		return resourceTypes[(int) resource];
	}
}
