#include <source\Game.h>

#include <source\scene\setup\Setup.h>
#include <source\scene\settings\SettingsScene.h>
#include <source\scene\snake\SnakeScene.h>


int main()
{
    tgui::TGUI_TabKeyUsageEnabled = true;

    Game* game = new Game();

    SnakeScene::create();

    SceneHandler::instance()->pushScene(Setup::instance());
    game->run();    
}