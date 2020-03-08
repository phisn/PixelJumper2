#pragma once

#include "Component.h"
#include "EntityView.h"

#include "Resource/WorldResource.h"

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
				component->initialize(this);
		}

		template <typename T>
		void registerComponent(T* const component)
		{
			accessComponents[component->getID()] = (void*) component;
			components[component->getID()] = component;
		}

	private:
		std::vector<Component*> components;
	};
}
