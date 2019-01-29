#pragma once

#include <Client/source/editor/manipulator/cache/Cache.h>
#include <Client/source/editor/manipulator/Component.h>
#include <Client/source/editor/manipulator/Element.h>

#include <Client/source/editor/EditorWorld.h>

#include <vector>

namespace Editor
{
	class CacheManager
	{
	public:
		CacheManager(
			Cache* const cache)
			:
			cache(cache)
		{
		}

		void registerComponent(
			const Cache::Sector sector,
			Component* const component)
		{
			// if doesnt exist -> automatically created
			components[sector].push_back(component);
		}

		void registerElement(
			const Cache::Sector sector,
			Element* const element)
		{
			elements[sector] = element;
		}

		void notify(
			const Cache::Sector sector)
		{
			if ( notifyElement(sector) )
			{
				notifyComponents(
					sector);
			}
		}

	private:
		void notifyComponents(
			const Cache::Sector sector)
		{
			for (Component* const component : components[sector])
			{
				component->notify(sector);
			}
		}

		bool notifyElement(
			const Cache::Sector sector)
		{
			decltype(elements)::iterator iter = elements.find(sector);

			if (iter != elements.end())
			{
				return iter->second->manipulate(
					&cache->input,
					&cache->output,

					sector);
			}

			return true;
		}

		std::map<
			Cache::Sector, 
			std::vector<Component*>
		> components;
		std::map<
			Cache::Sector,
			Element*
		> elements;

		Cache* const cache;
	};
}
