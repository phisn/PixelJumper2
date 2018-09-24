#pragma once

#include "Direction.h"

#include <SFML\Graphics.hpp>
#include <iostream>

class SnakeBody
{
public:
    SnakeBody(
        const sf::Vector2i position,
        const Direction direction,
        const sf::Color& color);
    ~SnakeBody();

    void increBodyPart();
    void decreBodyPart();
    void resetBodyPart();

    const sf::Vector2i& getPosition() const;
    const sf::Shape& getShape() const;
    void setNextDirection(Direction direction)
    {
        nextDirection = direction;
    }

    const sf::Vector2f& getRealSize() const
    {
        return bodyShape.getSize();
    }

    const Direction& getDirection() const
    {
        return direction;
    }

    static int BODYPART_LENGTH;
    static int block_size;
private:
    const sf::Color& color;
    bool isTail = false;

    void calcBodyPartPosition();
    int bodyPartCounter = 0;

    const sf::Vector2i position;
    const Direction direction;
    Direction nextDirection = Direction::INVALID;

    sf::RectangleShape bodyShape;
};

