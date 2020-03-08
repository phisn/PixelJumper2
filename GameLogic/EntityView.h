#pragma once

#include "Resource/ComponentResource.h"

namespace Game
{
	class EntityView
	{
	public:
		template <typename T>
		T* findComponent(const Resource::ComponentID componentID)
		{
			decltype(accessComponents)::iterator component = accessComponents.find(componentID);
			return component == accessComponents.end()
				? NULL
				: component->second;
		}

	protected:
		std::unordered_map<Resource::ComponentID, void*> accessComponents;
	};
}