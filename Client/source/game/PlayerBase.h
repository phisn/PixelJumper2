#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/game/CollisionEngine.h>
#include <Client/source/game/GameWorld.h>
#include <Client/source/game/PlayerState.h>
#include <Client/source/game/Simulator.h>

#include <set>
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
			// TODO: Split value into parts
			float timeValue = time.asMicroseconds() / 1000.f;

			do
			{
				const float timeValueStep = timeValue >= TIME_VALUE_STEP
					? TIME_VALUE_STEP
					: timeValue;

				applyGravity(
					timeValueStep
				);

				moveTo(
					state.Properties.position + getTargetOffset(timeValueStep)
				);

				timeValue -= timeValueStep;
			} while (timeValue > 0);

			updateVisual();
		}

		GameWorld* getWorld()
		{
			return currentWorld;
		}

	private:
		GameWorld* currentWorld; // adjust world if player finished
		const float TIME_VALUE_STEP = 1.f;

		sf::Vector2f getTargetOffset(const float timeValue) const
		{
			return	state.Properties.movement
				*	state.Properties.speed
				*	timeValue;
		}

		void applyGravity(const float timeValue)
		{
			// get gravity from occupying tiles
			// TODO: Implement correctly
			state.Properties.movement.y -= timeValue * state.Properties.speed;
		}

		struct CustomCollisionResult
		{
			bool collisionFound;
			CollisionEngine::CollisionInfo collisionInformation;
			GameTileBase* collisionTile;
		};

		void moveTo(sf::Vector2f targetPosition)
		{
			using CollidableTiles = std::vector<CollidableTile*>;
			using CollidableElement = std::pair<CollisionType, CollidableTiles>;

			/*
			
				Very unoptimized
			
			*/

			struct
			{
				CollidableTile* tile;
				const CollisionType* type;

				CollisionEngine::CollisionInfo info;
				float distance = 0x7fff'ffff;

			} closestCollision;

			closestCollision.tile = NULL; // == NOT_FOUND

			do
			{
				for (const CollidableElement& element : currentWorld->getTileContainer()->getSortedTiles().collidableTiles)
				{
					const CollisionEngine::CollisionContext collisionContext = CollisionEngine::SetupCollisionContext(
						state.Properties.position,
						targetPosition,
						element.first.invertTile,
						element.first.invertPlayer,
						element.first.isWeak);

					for (CollidableTile* const tile : element.second)
						if (CollisionEngine::FindCollision(
								&collisionContext,
								tile->getSize(),
								tile->getPosition() ))
						{
							const CollisionEngine::CollisionInfo currentInfo = CollisionEngine::GetLastCollision();

							const sf::Vector2f currentDistanceVector = state.Properties.position - currentInfo.position;
							const float currentDistance = sqrtf(
								currentDistanceVector.x * currentDistanceVector.x + // abs not needed (-1 * -1) == 1
								currentDistanceVector.y * currentDistanceVector.y
							);

							if (currentDistance < closestCollision.distance)
							{
								closestCollision.distance = currentDistance;
								closestCollision.tile = tile;
								closestCollision.info = currentInfo;
								closestCollision.type = &element.first;
							}
						}

					if (closestCollision.tile != NULL)
					{
						Collision collision;

						collision.info = closestCollision.info;
						collision.player = &state;
						collision.target = targetPosition;

						targetPosition = closestCollision.tile->onCollision(*closestCollision.type, collision);
					}
					else
					{
						state.Properties.position = targetPosition;
						
						break;
					}
				}

			} while (state.Properties.position != targetPosition);
		}

		void updateVisual()
		{
			Visual.basicShape.setPosition(state.Properties.position);
		}

		struct
		{
			sf::RectangleShape basicShape;

		} Visual;

		struct
		{
			GameTileBase* currentlyOccupying[4];

		} PrivateState;

		std::wstring name;
	};
}
