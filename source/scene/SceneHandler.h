#pragma once

class IScene;
class SceneHandler
{
public:
    void pushScene(IScene* scene);
    void popScene();

    static SceneHandler* instance()
    {
        return sceneHandler;
    }
private:
    static SceneHandler* sceneHandler;
};

