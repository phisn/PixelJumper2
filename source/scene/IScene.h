#pragma once

#include <SFML\Graphics.hpp>

#include "SceneHandler.h"

class IScene
{
public:
    void kill()
    {
        alive = false;
        onShutdown();
    }

    const bool isAlive() const
    {
        return alive;
    }

    const bool isFront() const
    {
        return front;
    }

    virtual void onShutdown() = 0;

    virtual void onEvent(sf::Event& event) = 0;
    virtual void onLogic(sf::Time& time) = 0;
    virtual void onDraw() = 0;
protected:
    bool alive = true;
private:
    friend SceneHandler;
    bool front = false;
};

