#include "Environment.h"
#include "Logger/Logger.h"

namespace Game
{
	bool Environment::initialize(const Resource::World* const worldResource)
	{
		for (const Resource::EntityResource& entityResource : worldResource->entities)
		{
			Entity* entity = new Entity;

			for (const Resource::ComponentResource& componentResource : worldResource->components)
				ComponentFactory::CreateComponent(entity, &componentResource);

			entities.push_back(entity);
		}

		for (Entity* const entity : entities)
			if (!entity->initialize())
			{
				return false;
			}

		for (Entity* const entity : entities)
			entity->registerTraits(this);

		return true;
	}

	bool Environment::initializeGraphics()
	{
		for (const StaticVisibleTrait& tile : staticVisibles)
		{
			sf::VertexBuffer vertexBuffer(sf::PrimitiveType::Quads);

			if (!vertexBuffer.create(4))
			{
				Log::Error(L"Failed to create vertex buffer");

				return false;
			}

			sf::Vertex vertices[4] = {};

			for (int i = 0; i < 4; ++i)
				vertices[i].color = tile.color;

			vertices[0].position = tile.position;
			vertices[1].position = tile.position + sf::Vector2f(tile.size.x, 0);
			vertices[2].position = tile.position + tile.size;
			vertices[3].position = tile.position + sf::Vector2f(0, tile.size.y);

			vertexBuffer.update((const sf::Vertex*) & vertices);
			vertexWorld.push_back(std::move(vertexBuffer));
		}

		for (DrawableTraitHandler* const handler : drawables)
			if (!handler->initializeGraphics())
			{
				return false;
			}

		return true;
	}
}
