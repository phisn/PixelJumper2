#include <Client/source/device/RootDevice.h>

#include "SceneHandler.h"

namespace Scene
{
	namespace
	{
		Type lastType = Type::Boot;
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
			DEVICE::Root::onTypeChanged(
				lastType,
				currentType);
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
				DEVICE::Root::onTypeChanged(
					lastType,
					currentType);
				lastType = currentType;
			}

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

	Type Handler::getCurrentType()
	{
		return scenes.back()->getType();
	}
}
