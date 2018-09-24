#include "scene/TestMenuScene.h"

int WINAPI wWinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	PWSTR pCmdLine, 
	int nCmdShow)
{
	Scene::Handler::pushScene( new Scene::TestMenu(0) );

	while (Scene::Handler::getSceneSize() > 0)
	{
		Scene::Handler::onDraw();
	}

	return 0;
}
