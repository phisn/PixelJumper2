#pragma once

#include "EntityView.h"

#include <unordered_map>

namespace Game
{
	class Component
	{
	public:
		virtual void initialize(EntityView* const entity) = 0;

		Resource::ComponentID getID() const
		{
			return id;
		}

	private:
		Resource::ComponentID id;
	};
}
