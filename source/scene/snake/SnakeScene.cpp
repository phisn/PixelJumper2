#include "SnakeScene.h"

SnakeScene* SnakeScene::snakeScene;

SnakeScene::SnakeScene()
{
    world = new World();

    border.setOutlineColor(
        sf::Color(sf::Color::Cyan));
    border.setOutlineThickness(64);
    border.setFillColor(sf::Color(sf::Color::Transparent));
    border.setPosition(0.f, 0.f);
    border.setSize(sf::Vector2f(
        world->getSize().x * SnakeBody::block_size,
        world->getSize().y * SnakeBody::block_size));
}

SnakeScene::~SnakeScene()
{
    delete world;
}

void SnakeScene::onShutdown()
{
    /*
    
        kick all players

    */
}

void SnakeScene::onEvent(
    sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed)
        for (LocalPlayer* localPlayer : world->getLocalPlayer())
        {
            localPlayer->getInput()->onKeyPressed(event.key.code);
        }
}

void SnakeScene::onLogic(
    sf::Time& time)
{
    for (LocalPlayer* localPlayer : world->getLocalPlayer())
    {
        localPlayer->update(time);
    }

    for (RemotePlayer* remotePlayer : world->getRemotePlayer())
    {
        remotePlayer->update(time);
    }
}

void SnakeScene::onDraw()
{
    for (LocalPlayer* localPlayer : world->getLocalPlayer())
    {
        Window::renderWindow()->setView(localPlayer->getView());

        for (const SnakeBody& snakeBody : localPlayer->getSnake()->getBodies())
        {
            Window::renderWindow()->draw(snakeBody.getShape());
        }
    }

    for (RemotePlayer* remotePlayer : world->getRemotePlayer())
        for (const SnakeBody& snakeBody : remotePlayer->getSnake()->getBodies())
        {
            Window::renderWindow()->draw(snakeBody.getShape());
        }

    Window::renderWindow()->draw(border);
}

void SnakeScene::create()
{
    snakeScene = new SnakeScene();
}

SnakeScene* SnakeScene::instance()
{
    return snakeScene;
}