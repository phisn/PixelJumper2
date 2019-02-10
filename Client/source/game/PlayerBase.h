#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/game/CollisionEngine.h>
#include <Client/source/game/GameWorld.h>
#include <Client/source/game/PlayerState.h>
#include <Client/source/game/Simulator.h>

#include <string>

#include <SFML/Graphics.hpp>

namespace Game
{
	class PlayerBase
	{
	public:
		PlayerState state;

		virtual void onDraw()
		{
			Device::Interface::GetScreen()->onDraw(&Visual.basicShape);
		}

		virtual void onLogic(const sf::Time time)
		{
			moveTo(
				state.Properties.position + getTargetOffset(time)
			);

			updateVisual();
		}

		GameWorld* getWorld()
		{
			return currentWorld;
		}

	private:
		GameWorld* currentWorld; // adjust world if player finished

		sf::Vector2f getTargetOffset(const sf::Time time) const
		{
			return	state.Properties.movement
				*	state.Properties.speed
				*	(time.asMicroseconds() / 1000.f
				);
		}

		void moveTo(sf::Vector2f targetPosition)
		{
			do
			{
				CollisionEngine::CollisionInformation collisionInformation = findCollisionTo(targetPosition);

				CollisionEngine::SetupLeaveCollisionContext(
					state.Properties.position,
					collisionInformation.position);

				// find leave collisions with new targetPosition (collision)
			} 
			while (true);
		}

		struct CustomCollisionResult
		{
			bool isCollisionFound;
			sf::Vector2f position;
			GameTileBase* const tile;

		} findCollisionTo(
			const sf::Vector2f targetPosition)
		{
			CollisionEngine::CollisionContext collisionContext = CollisionEngine::SetupEnterCollisionContext(
				state.Properties.position,
				targetPosition);

			CollisionEngine::CollisionInformation currentCollision = { };
			float closestDistance = 0x7fff'ffff;

			currentCollision.type = (CollisionEngine::CollisionInformation::Type) -1; // indicate not found

			for (GameTileBase* const tile : currentWorld->getSortedTiles().collidableTiles)
				if (CollisionEngine::FindEnterCollision(
					&collisionContext,
					tile->getSize(),
					tile->getPosition()
				))
				{
					const CollisionEngine::CollisionInformation tempCollision = CollisionEngine::GetLastCollision();

					const sf::Vector2f distanceVector = state.Properties.position - tempCollision.position;
					const float distance = sqrtf(
						distanceVector.x * distanceVector.x + // abs not needed (-1 * -1) == 1
						distanceVector.y * distanceVector.y
					);

					if (distance < closestDistance)
					{
						currentCollision = tempCollision;
						closestDistance = distance;
					}
				}


		}

		void updateVisual()
		{
			Visual.basicShape.setPosition(state.Properties.position);
		}

		struct
		{
			sf::RectangleShape basicShape;

		} Visual;

		std::wstring name;
	};
}
