#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ResourceDevice.h>
#include <Client/source/resource/StaticResource.h>

#include <SFML/System/MemoryInputStream.hpp>

#include <map>

namespace Framework
{
	class ResourceContext
	{
	public:
		typedef RESOURCE::Static::Resource Resource;
		typedef RESOURCE::Static::Type ResourceType;

		~ResourceContext()
		{
			for (const std::pair<ResourceType, Resource>& resource : resources)
			{
				delete[] resource.second.first;
			}
		}

		bool has(
			ResourceType type)
		{
			if (cache.type == type)
			{
				return true;
			}

			decltype(resources)::iterator it = resources.find(type);
			if (it == resources.end())
			{
				return false;
			}
			else
			{
				cache.type = it->first;
				cache.resource = &it->second;

				return true;
			}
		}

		sf::MemoryInputStream get(
			ResourceType type)
		{
			if (cache.type == type)
			{
				return getCache();
			}

			if ( !has(type) )
			{
				obtain(type);
			}

			return getCache();
		}

		sf::MemoryInputStream obtain(
			ResourceType type)
		{
			
			RESOURCE::Static::Resource resource = DEVICE::Interface::GetResource()->obtain(type);
			resources[type] = resource;

			sf::MemoryInputStream mis;
			mis.open(
				resource.first,
				resource.second);
			return mis;
		}

	private:
		sf::MemoryInputStream getCache()
		{
			sf::MemoryInputStream mis;
			mis.open(
				cache.resource->first, 
				cache.resource->second);
			return mis;
		}

		struct
		{
			Resource* resource;
			ResourceType type;

		} cache;

		std::map<ResourceType, Resource> resources;
	};
}
