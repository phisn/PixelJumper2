#pragma once

#include <Client/source/editor/manipulator/cache/Cache.h>
#include <Client/source/editor/manipulator/Component.h>
#include <Client/source/editor/manipulator/Element.h>
#include <Client/source/editor/World.h>

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
			notifyElements(
				sector);
			notifyComponents(
				sector);
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

		void notifyElements(
			const Cache::Sector sector)
		{
			decltype(elements)::iterator iter = elements.find(sector);

			if (iter != elements.end())
			{
				elements[sector]->manipulate(
					&cache->input,
					&cache->output,

					sector);
			}

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
