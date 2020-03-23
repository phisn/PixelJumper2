#pragma once

#include "EntityView.h"
#include "EnvironmentView.h"

#include <unordered_map>

namespace Game
{
	class Component
	{
	public:
		Component(const Resource::ComponentID id)
			:
			id(id)
		{
		}

		virtual bool initialize(EntityView* const entity) = 0;
		virtual void registerTraits(EnvironmentView* const environment) = 0;

		Resource::ComponentID getID() const
		{
			return id;
		}

	private:
		Resource::ComponentID id;
	};
}
