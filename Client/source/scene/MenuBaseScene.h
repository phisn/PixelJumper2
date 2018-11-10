#pragma once

#include <Client/source/menu/RootBase.h>

#include <Client/source/scene/MainSceneBase.h>

#include <vector>

namespace Scene
{
	class MenuBase
		:
		public MainBase
	{
	public:
		virtual ~MenuBase() { };

		virtual void onEvent(
			const sf::Event event)
		{
			if (event.type == sf::Event::MouseButtonPressed &&
				event.mouseButton.button == sf::Mouse::Button::Left)
			{
				selected->getView()->getSize();
			}

			selected->onEvent(event);
		}

		virtual void onLogic(
			const sf::Time time)
		{
			selected->onLogic(time);
		}

		virtual void onDraw()
		{
			selected->onDraw();
		}

	protected:
		Menu::RootBase* selected;

		void addRoot(
			Menu::RootBase* const root)
		{
			containers.push_back(root);
		}

		const std::vector<Menu::RootBase*>& getContainers() const
		{
			return containers;
		}

	private:
		std::vector<Menu::RootBase*> containers;
	};
}