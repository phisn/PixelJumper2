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
			CollisionEngine::CollisionInformation collisionInformation;
			GameTileBase* collisionTile;
		};

		void moveTo(const sf::Vector2f targetPosition)
		{
			while (true)
			{
				const CustomCollisionResult result = findEnterCollisionTo(targetPosition);
				
				if (result.collisionFound)
				{
					handleLeaveCollisionEventsTo(result.collisionInformation.position);
					handleEnterCollisionEvent(result);

					if (result.collisionInformation.remainingDistance == NULL)
					{
						break;
					}
				}
				else
				{
					handleLeaveCollisionEventsTo(targetPosition);

					break;
				}
			}
		}

		// handle collision events and change player state
		void handleEnterCollisionEvent(
			const CustomCollisionResult result)
		{
			// TODO:
		}

		// does collision finding and event handling in one
		void handleLeaveCollisionEventsTo(const sf::Vector2f targetPosition)
		{ // TODO: can only be a single?
			const CollisionEngine::CollisionContext collisionContext = CollisionEngine::SetupLeaveCollisionContext(
				state.Properties.position,
				targetPosition);
			
			using LeavedTile = std::tuple<float, CollisionEngine::CollisionInformation, GameTileBase*>;
			using LeavedTileLess = bool (*)(const LeavedTile& l1, const LeavedTile& l2);

			std::set<LeavedTile, LeavedTileLess> leavedTiles(
				[](const LeavedTile& l1, const LeavedTile& l2) -> bool
			{
					return std::get<float>(l1) < std::get<float>(l2);
			});

			for (GameTileBase* const tile : currentWorld->getSortedTiles().leaveableTiles)
				if (CollisionEngine::FindLeaveCollision(
						&collisionContext,
						tile->getSize(),
						tile->getPosition() ))
				{
					const CollisionEngine::CollisionInformation currentCollision = CollisionEngine::GetLastCollision();
					const sf::Vector2f currentDistanceVector = state.Properties.position - currentCollision.position;
					
					leavedTiles.insert(std::make_tuple(
						sqrtf(
							currentDistanceVector.x * currentDistanceVector.x + // abs not needed (-1 * -1) == 1
							currentDistanceVector.y * currentDistanceVector.y
						),
						currentCollision,
						tile
					));
				}

			for (const LeavedTile& leavedTile : leavedTiles)
			{
				const CollisionEngine::CollisionInformation collisionInformation = std::get<CollisionEngine::CollisionInformation>(leavedTile);
				GameTileBase* tile = std::get<GameTileBase*>(leavedTile);

				// TODO: have fun (call events [leave])
			}
		}

		CustomCollisionResult findEnterCollisionTo(const sf::Vector2f targetPosition)
		{
			CustomCollisionResult result = { }; // init to zero
			const CollisionEngine::CollisionContext collisionContext = CollisionEngine::SetupEnterCollisionContext(
				state.Properties.position,
				targetPosition);

			CollisionEngine::CollisionInformation closestCollision;
			float closestDistance;

			for (GameTileBase* const tile : currentWorld->getSortedTiles().collidableTiles)
				if (CollisionEngine::FindEnterCollision(
					&collisionContext,
					tile->getSize(),
					tile->getPosition() ))
				{
					const CollisionEngine::CollisionInformation currentCollision = CollisionEngine::GetLastCollision();

					const sf::Vector2f currentDistanceVector = state.Properties.position - currentCollision.position;
					const float currentDistance = sqrtf(
						currentDistanceVector.x * currentDistanceVector.x + // abs not needed (-1 * -1) == 1
						currentDistanceVector.y * currentDistanceVector.y
					);

					if (result.collisionFound)
					{
						if (currentDistance < closestDistance)
						{
							result.collisionTile = tile;
							result.collisionInformation = currentCollision;

							closestDistance = currentDistance;
						}
					}
					else
					{
						result.collisionFound = true;
						result.collisionTile = tile;
						result.collisionInformation = currentCollision;

						closestDistance = currentDistance;
					}
				}

			return result;
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
