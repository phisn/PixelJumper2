#pragma once

#include "ResourceBase.h"

namespace Resource
{
	enum class ComponentID : sf::Uint16
	{
		CommonExit,
		DynamicWorldTransition,
		SolidCollision
	};

	class ComponentResource
		:
		public ResourceBase
	{
	public:
		static ComponentResource* Create(const ComponentID componentID);
		
		ComponentResource(const ComponentID id)
			:
			id(id)
		{
		}

		virtual ~ComponentResource()
		{
		}

		ComponentID getID() const
		{
			return id;
		}

	private:
		const ComponentID id;
	};
}
