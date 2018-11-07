#pragma once

#include <Client/source/editor/manipulator/Cache.h>
#include <Client/source/editor/manipulator/Component.h>
#include <Client/source/editor/World.h>

#include <vector>

namespace Editor
{
	class CacheManipulator
	{
	public:
		// Task to manipulate cache (ex. undo)
		virtual bool manipulate(
			Cache::Input* input,
			Cache::Output* output) = 0;
	};

	class Element;
	class CacheManager
	{
		friend Element;
	public:
		CacheManager(
			Cache* const cache)
			:
			cache(cache)
		{
		}

		void manipulate(
			CacheManipulator* manipulator)
		{
			manipulator->manipulate(
				&cache->input,
				&cache->output);

			notifyComponents();
		}

		void registerComponent(
			Component* const component)
		{
			components.push_back(component);
		}

		// normally not callable
		void notifyComponents()
		{
			for (Component* component : components)
			{
				component->notify();
			}
		}

	private:
		std::vector<Component*> components;

		// called by elements
		void notify()
		{
			// calculate cache things
		}

		Cache* const cache;
	};
}