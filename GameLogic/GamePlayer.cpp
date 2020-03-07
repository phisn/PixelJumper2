#include "GamePlayer.h"
 
float Game::PlayerBase::getSingleJumpForce(const CollisionEngine::CollisionInfo::Type type) const
{
	return collisionContainer.isActive(type)
		? collisionContainer.readCollider(type).jumpForce
		: 0;
}

float Game::PlayerBase::getSingleForceAddition(const CollisionEngine::CollisionInfo::Type type) const
{
	return collisionContainer.isActive(type)
		? collisionContainer.readCollider(type).inputForceAddition
		: 1;
}
