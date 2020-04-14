#pragma once

#include "Logger/Logger.h"
#include "ResourceBase.h"

namespace Resource
{
	enum class ComponentID : sf::Uint16
	{
		DynamicWorldTransition,
		CommonExit,
		SolidCollision,
		StaticVisible,
		Tile,
	};

	class ComponentResource
		:
		public ResourceBase
	{
	public:
		ComponentResource(ComponentResource&& other)
		{
			componentID = other.componentID;
			instance = other.instance;
			resource = other.resource;

			other.instance = NULL;
			other.resource = NULL;
		}

		ComponentResource& operator=(ComponentResource&& other)
		{
			componentID = other.componentID;
			instance = other.instance;
			resource = other.resource;
			
			other.instance = NULL;
			other.resource = NULL;

			return *this;
		}

		// passing responsibility for instance
		template <typename Component>
		ComponentResource(
			const ComponentID componentID,
			Component* const instance)
			:
			componentID(componentID),
			resource((ResourceBase*) instance),
			instance((void*) instance)
		{
		}

		ComponentResource()
			:
			resource(NULL)
		{
		}

		~ComponentResource()
		{
			Log::Warning(L"Component deleted");

			if (resource)
				delete resource;
		}

		bool make(ReadPipe* const pipe) override
		{
			if (!pipe->readValue(&componentID))
			{
				return false;
			}

			if (!initiate(componentID))
			{
				return false;
			}

			return resource->make(pipe);
		}

		bool save(WritePipe* const pipe) override
		{
			if (!pipe->writeValue(&componentID))
			{
				return false;
			}

			return resource->save(pipe);
		}

		template <typename Component>
		Component* getInstance() const
		{
			return (Component*) instance;
		}

		ComponentID getID() const
		{
			return componentID;
		}
		
	private:
		bool initiate(const ComponentID componentID);

		template <typename Component>
		void initiateFromComponent(Component* const instance)
		{
			this->instance = (void*) instance;
			resource = (ResourceBase*) instance;
		}

		ComponentID componentID;
		void* instance;
		Resource::ResourceBase* resource = NULL;
	};
}
