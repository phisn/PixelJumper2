#include "SceneHandler.h"

#include <iostream>
namespace Scene
{
	namespace
	{
		std::vector<Base*> scenes;
	}

	bool Handler::pushScene(
		Base* scene)
	{
		if (!scene->onCreate())
		{
			delete scene;

			return false;
		}

		if (scenes.size() > 0)
		{
			scenes.back()->onHide();
		}

		scenes.push_back(scene);
		scene->onShow();

		return true;
	}

	bool Handler::popScene()
	{
		if (!scenes.back()->onRemove())
		{
			return false;
		}

		scenes.back()->onHide();

		delete scenes.back();
		scenes.pop_back();

		if (scenes.size() > 0)
		{
			scenes.back()->onShow();
		}

		return true;
	}

	void Handler::onDraw()
	{
		scenes.back()->onDraw();
	}

	void Handler::onLogic(
		sf::Time time)
	{
		scenes.back()->onLogic(time);
	}

	void Handler::onEvent(
		sf::Event event)
	{
		scenes.back()->onEvent(event);
	}

	size_t Handler::getSceneSize()
	{
		return scenes.size();
	}
}
