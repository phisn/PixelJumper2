#pragma once

#include "Component.h"
#include "EntityView.h"

#include "ResourceCore/WorldResource.h"

#include <cassert>
#include <vector>

namespace Game
{
	class Entity
		:
		public EntityView
	{
	public:
		~Entity()
		{
			for (Component* const component : components)
				delete component;
		}

		bool initialize()
		{
			for (Component* const component : components)
				if (!component->initialize(this))
				{
					return false;
				}

			return true;
		}

		template <typename T>
		void registerComponent(T* const component)
		{
			accessComponents[component->getID()] = (void*) component;
			components.push_back(component);
		}


		void registerTraits(EnvironmentView* const environment)
		{
			for (Component* const component : components)
				component->registerTraits(environment);
		}

	private:
		std::vector<Component*> components;
	};
}
