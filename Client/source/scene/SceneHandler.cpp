#include "SceneHandler.h"

#include <iostream>
namespace Scene
{
	namespace
	{
		Type lastType = Type::Boot;
		OnTypeChanged onTypeChanged;
		
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

		if (scene->getType() != lastType)
		{
			const Type currentType =
				scene->getType();
			if (onTypeChanged)
			{
				onTypeChanged(
					lastType,
					currentType);
			}
			lastType = currentType;
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
			if (scenes.back()->getType() != lastType)
			{
				const Type currentType = 
					scenes.back()->getType();
				onTypeChanged(
					lastType,
					currentType);
				lastType = currentType;
			}

			scenes.back()->onShow();
		}

		return true;
	}

	void Handler::onDraw(
		::Device::Screen* window)
	{
		scenes.back()->onDraw(window);
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

	Type Handler::getCurrentType()
	{
		return scenes.back()->getType();
	}

	void Handler::setOnTypeChanged(
		OnTypeChanged onTypeChanged)
	{
		::Scene::onTypeChanged = onTypeChanged;
	}
}
