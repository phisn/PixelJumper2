#include <source\scene\SceneHandler.h>
#include <source\Game.h>
#include <source\scene\IScene.h>

SceneHandler* SceneHandler::sceneHandler;

void SceneHandler::pushScene(
    IScene* scene)
{
    if (Game::game->sceneStack.size() > 0)
        Game::game->sceneStack.top()->front = false;
    Game::game->sceneStack.push(scene);
    Game::game->sceneStack.top()->front = true;
}

void SceneHandler::popScene()
{
    if (Game::game->sceneStack.size() > 0)
    {
        Game::game->sceneStack.top()->onShutdown();

        Game::game->sceneStack.top()->front = false;
        Game::game->sceneStack.pop();

        if (Game::game->sceneStack.size() > 0)
            Game::game->sceneStack.top()->front = true;
    }
}
