#include "Player.h"
#include "World.h"

World* Player::world;

Player::Player()
{
}

Player::~Player()
{
}

void Player::update(sf::Time& time)
{
    this->time += time;

    while (this->time > speed)
    {
        this->time -= speed;

        onSnakeMove();
    }

    snake->update(time);
}
