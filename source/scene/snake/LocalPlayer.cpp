#include "LocalPlayer.h"
#include <source\Window.h>
#include "World.h"

LocalPlayer::LocalPlayer(
    Input* input)
    :
    input(input)
{
    snake = new Snake(sf::Vector2i(
        rand() % world->getSize().x,
        rand() % world->getSize().y),
        DIRECTION_RANDOM,
        color);

    snake->bindPlayer(this);

    changeSpeed(sf::microseconds(70000));
    updateView();

    updateZoom(64);
}


LocalPlayer::~LocalPlayer()
{
    delete input;
}

void LocalPlayer::onSnakeMove()
{
    switch (input->pullKey())
    {
    case Key_Type::Up:
        snake->changeDirection(Direction::UP);

        break;
    case Key_Type::Left:
        snake->changeDirection(Direction::LEFT);

        break;
    case Key_Type::Down:
        snake->changeDirection(Direction::DOWN);

        break;
    case Key_Type::Right:
        snake->changeDirection(Direction::RIGHT);

        break;
    }

    snake->moveSnake();
}

void LocalPlayer::updateView()
{
    if (DIRECTION_IS_LEFT_OR_UP(snake->head().getDirection()))
    {
        view.setCenter(
            snake->head().getShape().getPosition().x,
            snake->head().getShape().getPosition().y);
    }
    else
    {
        view.setCenter(
            snake->head().getShape().getPosition().x - SnakeBody::block_size
                + snake->head().getRealSize().x,
            snake->head().getShape().getPosition().y - SnakeBody::block_size
                + snake->head().getRealSize().y);
    }
}

void LocalPlayer::updateZoom(float zoom)
{
    view.setSize(
        SnakeBody::block_size * zoom * Window::renderWindow()->getSize().x / Window::renderWindow()->getSize().y,
        SnakeBody::block_size * zoom);
}