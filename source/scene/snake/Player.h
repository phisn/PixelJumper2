#pragma once

#include "Input.h"
#include "Snake.h"

#include <SFML\Graphics.hpp>

class World;
class Player
{
public:
    Player();
    ~Player();

    void update(sf::Time& time);

    Snake* getSnake() const
    {
        return snake;
    }

    void changeSpeed(
        sf::Time speed)
    {
        snake->changeSpeed(speed);

        this->speed = speed;
    }

    void setColor(sf::Color color)
    {
        this->color = color;
    }

    void setName(std::string name)
    {
        this->name = name;
    }
protected:
    friend World;
    virtual void onSnakeMove() = 0;

    static World* world;
    Snake* snake;

    sf::Time speed;
    sf::Time time;

    sf::Color color = sf::Color::Red;
    std::string name;
};

