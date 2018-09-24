#pragma once

#include <source\scene\IScene.h>
#include <source\Window.h>
#include <source\scene\snake\World.h>

class SnakeScene : public IScene
{
public:
    SnakeScene();
    ~SnakeScene();

    virtual void onShutdown() override;
    virtual void onEvent(sf::Event & event) override;
    virtual void onLogic(sf::Time & time) override;
    virtual void onDraw() override;

    static void create();
    static SnakeScene* instance();
private:
    static SnakeScene* snakeScene;
    World* world;

    sf::RectangleShape border;
};

