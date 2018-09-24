#pragma once

#include "Direction.h"
#include "SnakeBody.h"

#include <SFML\Graphics.hpp>
#include <deque>

class LocalPlayer;
class Snake
{
public:
    Snake(
        sf::Vector2i position,
        Direction direction,
        sf::Color& color);
    ~Snake();

    const SnakeBody& head() const
    {
        return bodies.front();
    }

    const SnakeBody& tail() const
    {
        return bodies.back();
    }

    void changeSpeed(sf::Time speed);
    void changeDirection(Direction direction);

    void moveSnake();
    void eat(int count = 1);
    void update(sf::Time time);

    void bindPlayer(LocalPlayer* localPlayer)
    {
        this->localPlayer = localPlayer;
    }
    
    const std::deque<SnakeBody>& getBodies() const
    {
        return bodies;
    }
protected:
    const sf::Vector2i&& move(const sf::Vector2i& position) const;

    const sf::Color& color;
    Direction direction;
    int foodCount = 1200;

    sf::Time speed_bp;
    sf::Time speed;
    sf::Time time;

    LocalPlayer* localPlayer = NULL;
    std::deque<SnakeBody> bodies;
};

