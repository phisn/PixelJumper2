#include <Client/source/device/DeviceInterface.h>
#include <Client/source/scene/SceneBase.h>

#include "SceneInterface.h"

#include <vector>

namespace
{
	std::vector<SCENE::Base*> scenes;
	SCENE::Type lastType;
}

namespace Scene
{
	void Interface::fallback(
		FallbackType type)
	{
		while (scenes.size() > 0 && !scenes.back()->isCatching())
		{
			scenes.back()->onRemove();

			delete scenes.back();
			scenes.pop_back();
		}

		if (scenes.size() > 0)
		{
			// If scene wants then it will do it itself
			scenes.back()->onCatch(type);
		}
	}

	bool Interface::pushScene(
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

		const Type type = scenes.back()->getType();
		if (type != lastType)
		{
			DEVICE::Interface::onSceneTypeChanged(
				lastType,
				type);

			lastType = type;
		}

		scenes.push_back(scene);
		scene->onShow();

		return true;
	}

	void Interface::popScene()
	{
		scenes.back()->onRemove();

		delete scenes.back();
		scenes.pop_back();

		if (scenes.size() == 0)
		{
			return;
		}

		const Type type = scenes.back()->getType();
		if (type != lastType)
		{
			DEVICE::Interface::onSceneTypeChanged(
				lastType,
				type);

			lastType = type;
		}
	}

	void Interface::onDraw()
	{
		scenes.back()->onDraw();
	}

	void Interface::onLogic(
		sf::Time time)
	{
		scenes.back()->onLogic(time);
	}

	void Interface::onEvent(
		sf::Event event)
	{
		scenes.back()->onEvent(event);
	}

	size_t Interface::getSceneSize()
	{
		return scenes.size();
	}

	Type Interface::getCurrentType()
	{
		return scenes.back()->getType();
	}
}