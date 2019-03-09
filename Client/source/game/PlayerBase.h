#pragma once

#include <Client/source/device/ScreenDevice.h>

#include <Client/source/game/PlayerState.h>

#include <Client/source/resource/PlayerResource.h>

#include <functional>
#include <string>
#include <unordered_set>

#include <SFML/Graphics.hpp>

namespace Game
{
	class PlayerBase
		:
		public GameState
	{
	public:
		PlayerState state;

		virtual void draw()
		{
			Device::Screen::Draw(shape);
		}

		bool writeState(
			Resource::WritePipe* const writePipe) override
		{
			return state.writeState(writePipe);
		}

		bool readState(
			Resource::ReadPipe* const readPipe) override
		{
			if (state.readState(readPipe))
			{
				initializeFromState();

				return true;
			}
			else
			{
				return false;
			}
		}
		                                                                                      
		

	protected:
		virtual void initializeFromState()
		{
			shape.setPosition(state.readProperties()->position);
			shape.setSize({ 1, 1 });
		}

	private:
		sf::RectangleShape shape;
	};

	class _N_PlayerBase
	{
	public:
		_N_PlayerState state;

		virtual void onDraw()
		{
			Device::Screen::Draw(Visual.basicShape);
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

				applyDrag(
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

	protected:
		GameWorld* currentWorld; // adjust world if player finished

	private:
		const float TIME_VALUE_STEP = 1.f;

		sf::Vector2f getTargetOffset(const float timeValue) const
		{
			return	state.Properties.movement
				*	state.Properties.speed
				*	timeValue;
		}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
		void applyGravity(const float timeValue)
		{
			const sf::Vector2f oldValue = state.Properties.movement;
			state.Properties.movement += currentWorld->worldState.gravity.getValue();
			state.movement.callListeners(oldValue);
		}

		void applyDrag(const float timeValue)
		{
			const sf::Vector2f oldValue = state.Properties.movement;
			const float lostMovementPercent = _N_Simulator::CalculateDrag(
				state.Properties.drag + currentWorld->worldState.drag,
				state.Properties.weight);

			state.Properties.movement *= lostMovementPercent;

			state.movement.callListeners(oldValue);
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
						state.position = targetPosition;
						
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

		std::wstring name;
	};
}
