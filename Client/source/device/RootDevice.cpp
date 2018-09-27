#include "RootDevice.h"

namespace Device
{
	bool Root::initialize()
	{
		if (!window.initialize())
		{
			return false;
		}

		if (!input.initialize())
		{
			return false;
		}

		if (!Scene::Handler::pushScene(
				new Scene::Boot))
		{
			return false;
		}

		return true;
	}

	int Root::start()
	{
		sf::Clock clock;
		sf::Event event;

		while (Scene::Handler::getSceneSize() > 0)
		{
			switch (Scene::Handler::getCurrentType())
			{
			case Scene::Type::Boot:
				Scene::Handler::onDraw(NULL);

				break;
			case Scene::Type::SFML:
				while (window.pollEvent(&event))
				{
					Scene::Handler::onEvent(event);
				}

				Scene::Handler::onLogic(
					clock.restart());

				window.clear();
				Scene::Handler::onDraw(
					&window);
				window.display();

				break;
			case Scene::Type::Win:
				Scene::Handler::onDraw(NULL);

				break;
			}
		}

		return 0;
	}
}
