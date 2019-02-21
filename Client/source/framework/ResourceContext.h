#pragma once

#include <Client/source/resource/ResourceInterface.h>

#include <SFML/System/MemoryInputStream.hpp>

#include <map>

namespace Framework
{
	class ResourceContext
	{
	public:
		typedef ::Resource::Static::Resource _Resource;
		typedef ::Resource::Static::Type _ResourceType;

		~ResourceContext()
		{
			for (const std::pair<_ResourceType, _Resource>& resource : resources)
			{
				delete[] resource.second.first;
			}
		}

		bool has(
			_ResourceType type)
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
			_ResourceType type)
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
			_ResourceType type)
		{
			
			::Resource::Static::Resource resource = ::Resource::Interface::GetStaticResource(type);
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
			_Resource* resource;
			_ResourceType type;

		} cache;

		std::map<_ResourceType, _Resource> resources;
	};
}
