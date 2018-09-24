#include "SnakeBody.h"

int SnakeBody::block_size = 64;
int SnakeBody::BODYPART_LENGTH = 16;

SnakeBody::SnakeBody(
    const sf::Vector2i position, 
    const Direction direction,
    const sf::Color& color)
    :
    position(position),
    direction(direction),
    color(color)
{
    bodyShape.setFillColor(color);
    calcBodyPartPosition();

    bodyPartCounter = 0;
}

SnakeBody::~SnakeBody()
{
}

void SnakeBody::increBodyPart()
{
    if (isTail) isTail = false;

    if (bodyPartCounter <= BODYPART_LENGTH)
    {
        ++bodyPartCounter;
    }

    calcBodyPartPosition();
}

void SnakeBody::decreBodyPart()
{
    if (!isTail)
        isTail = true;

    if (bodyPartCounter > 0)
    {
        --bodyPartCounter;
    }

    calcBodyPartPosition();
}

void SnakeBody::resetBodyPart()
{
    std::cout << bodyShape.getSize().x << "x | y" << bodyShape.getSize().y << std::endl;
    bodyPartCounter = BODYPART_LENGTH;

    bodyShape.setPosition(
        position.x * block_size,
        position.y * block_size);
    bodyShape.setSize(sf::Vector2f(
        block_size,
        block_size));
}

const sf::Vector2i& SnakeBody::getPosition() const
{
    return position;
}

const sf::Shape& SnakeBody::getShape() const
{
    return bodyShape;
}

void SnakeBody::calcBodyPartPosition()
{
    Direction directionUse = nextDirection == Direction::INVALID
        ? direction
        : nextDirection;
    double length = (block_size / BODYPART_LENGTH) * bodyPartCounter;

    switch (isTail ? DIRECTION_REVERSE(directionUse) : directionUse)
    {
    case Direction::DOWN:
        bodyShape.setPosition(
            block_size * position.x,
            block_size * position.y);
        bodyShape.setSize(sf::Vector2f(
            block_size,
            length));

        break;
    case Direction::RIGHT:
        bodyShape.setPosition(
            block_size * position.x,
            block_size * position.y);
        bodyShape.setSize(sf::Vector2f(
            length,
            block_size));

        break;
    case Direction::LEFT:
        bodyShape.setPosition(
            block_size * position.x + (block_size - length),
            block_size * position.y);
        bodyShape.setSize(sf::Vector2f(
            length,
            block_size));

        break;
    case Direction::UP:
        bodyShape.setPosition(
            block_size * position.x,
            block_size * position.y + (block_size - length));
        bodyShape.setSize(sf::Vector2f(
            block_size,
            length));

        break;
    }
}
