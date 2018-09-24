#pragma once

#include <source\scene\IScene.h>
#include <source\scene\SceneHandler.h>
#include <source\Window.h>

#include <SFML\Graphics.hpp>
#include <stack>
#include <iostream>

/*

Setup -> Game -> Settings
         Game <- Settings
Setup <- Game
Exit
*/

class Game
{
public:
    Game();
    ~Game();

    void run();
    void loadWinConfig();
private:
    friend SceneHandler;
    static Game* game;

    Window* window;
    std::stack<IScene*> sceneStack;
};

