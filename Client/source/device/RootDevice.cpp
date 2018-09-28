#include "RootDevice.h"

namespace Device
{
	bool Root::initialize()
	{
		if (!Window::initialize())
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
				Scene::Handler::onDraw();

				break;
			case Scene::Type::SFML:
				while (Window::pollEvent(&event))
				{
					Scene::Handler::onEvent(event);
				}

				Scene::Handler::onLogic(
					clock.restart());

				Window::clear();
				Scene::Handler::onDraw();
				Window::display();

				break;
			case Scene::Type::Win:
				Scene::Handler::onDraw();

				break;
			}
		}

		return 0;
	}

	void Root::onTypeChanged(
		Scene::Type lastType,
		Scene::Type currentType)
	{
		switch (lastType)
		{
		case Scene::Type::SFML:
			Window::hide();

			break;
		}

		switch (currentType)
		{
		case Scene::Type::SFML:
			Window::show();

			break;
		}
	}
}
