#include "Snake.h"
#include "LocalPlayer.h"

Snake::Snake(
    sf::Vector2i position,
    Direction direction,
    sf::Color& color)
    :
    color(color)
{
    bodies.push_front(SnakeBody(
        position,
        direction,
        color));
}

Snake::~Snake()
{
}

void Snake::changeSpeed(
    sf::Time speed)
{
    this->speed = speed;
    this->speed_bp = sf::microseconds(
        speed.asMicroseconds() / SnakeBody::BODYPART_LENGTH);
}

void Snake::changeDirection(
    Direction direction)
{
    this->direction = direction;
}

void Snake::moveSnake()
{
    bodies.front().resetBodyPart();
    bodies.front().setNextDirection(direction);
    bodies.push_front(SnakeBody(
        move(
            head().getPosition()),
        direction,
        color));

    if (foodCount == 0)
    {
        bodies.pop_back();
    }
    else --foodCount;
}

void Snake::eat(int count)
{
    foodCount += count;
} 

void Snake::update(
    sf::Time time)
{
    this->time += time;

    while (this->time > speed_bp)
    {
        this->time -= speed_bp;

        bodies.front().increBodyPart();
        bodies.back().decreBodyPart();

        if (localPlayer)
            localPlayer->updateView();
    }
}

const sf::Vector2i && Snake::move(
    const sf::Vector2i & position) const
{
    sf::Vector2i temp(position);

    switch (direction)
    {
    case Direction::DOWN:
        ++temp.y;

        break;
    case Direction::RIGHT:
        ++temp.x;

        break;
    case Direction::LEFT:
        --temp.x;

        break;
    case Direction::UP:
        --temp.y;

        break;
    }

    return std::move(temp);
}
