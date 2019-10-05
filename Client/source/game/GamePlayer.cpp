#include "GamePlayer.h"
#include <Client/source/game/tiletrait/CollidableTile.h>

float Game::PlayerBase::getSingleJumpForce(const CollisionEngine::CollisionInfo::Type type) const
{
	return collisionContainer.has(type)
		? collisionContainer[type]->getDensity()
		: 0;
}

float Game::PlayerBase::getSingleForceAddition(const CollisionEngine::CollisionInfo::Type type) const
{
	return collisionContainer.has(type)
		? collisionContainer[type]->getInputForceAddition()
		: 1;
}
