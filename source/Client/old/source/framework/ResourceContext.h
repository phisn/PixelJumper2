#pragma once

#include <Client/source/logger/Logger.h>
#include <Client/source/resource/ResourceInterface.h>

#include <any>
#include <map>

#include <SFML/Graphics/Font.hpp>
#include <SFML/System/MemoryInputStream.hpp>

namespace Framework
{
	class ResourceContext
	{
		typedef Resource::Static::Id _ResourceType;
		typedef std::pair<Resource::Static::Resource, std::any> _Resource;

	public:
		bool has(const _ResourceType type) const
		{
			return !!resources.count(type);
		}

		_Ret_maybenull_
		template <typename T>
		const T* get(const _ResourceType type)
		{
			decltype(resources)::const_iterator iterator = resources.find(type);

			if (iterator == resources.cend())
			{
				return obtain(type);
			}
			else
			{
				return std::any_cast<const T>(&iterator->second);
			}
		}

		template <typename T>
		const T* read(const _ResourceType type) const
		{
			return std::any_cast<const T>(&resources.at(type).second);
		}

		_Ret_maybenull_
		template <typename T>
		const T* obtain(const _ResourceType type)
		{
			return emplace<T>(type, Resource::Interface::GetStaticResource(type));
		}

	private:
		_Ret_maybenull_
		template <typename T>
		const T* emplace(
			const _ResourceType type,
			const Resource::Static::Resource resource)
		{
			Log::Error(L"Invalid type in (ResourceContext -> emplace)");
		}

		_Ret_maybenull_
		template<>
		const sf::Font* emplace<sf::Font>(
			const _ResourceType type,
			const Resource::Static::Resource resource)
		{
			sf::Font font;

			if (!font.loadFromMemory(resource.first, resource.second))
			{
				Log::Error(L"Failed to load font");
			
				// try somehow to fix it
				
				return NULL;
			}

			return std::any_cast<const sf::Font>(
				&(resources[type]
					= std::make_pair(resource, std::move(font))
				 ).second);
		}

		std::map<_ResourceType, _Resource> resources;
	};
}
