#pragma once


#include "Component.h"
#include "ComponentFactory.h"
#include "Entity.h"

#include "Resource/WorldResource.h"

#include "trait/CollidableTrait.h"
#include "trait/DrawableTrait.h"


#include "CollisionEngine.h"
#include "trait/DynamicWorldEntry.h"

#include <cassert>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>

namespace Game
{
	class Environment
		:
		public EnvironmentView
	{
	public:
		bool initialize(const Resource::World* const worldResource);
		bool initializeGraphics();

		void draw(sf::RenderTarget* const target) const
		{
			for (const sf::VertexBuffer& vertexTile : vertexWorld)
				target->draw(vertexTile);

			for (DrawableTraitHandler* const handler : drawables)
				handler->onDraw(target);
		}

		void processLogic()
		{
			for (DynamicTrait* const dynamic : dynamics)
				dynamic->processLogic();
		}

	private:
		std::vector<Entity*> entities;
		std::vector<sf::VertexBuffer> vertexWorld;
	};
}
