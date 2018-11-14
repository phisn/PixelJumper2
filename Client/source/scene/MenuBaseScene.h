#pragma once

#include <Client/source/menu/RootBase.h>

#include <Client/source/scene/MainSceneBase.h>

#include <vector>
#include <iostream>

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
			/*
			
				Create strong select and weak select
				Strong: MouseButtonPressed -> for important events
				Weak:   MouseMoved -> for visual events
			
			*/
			if (event.type == sf::Event::MouseButtonPressed &&
				!isInside(
					selected,

					event.mouseButton.x,
					event.mouseButton.y)
				)
			{
				for (MENU::RootBase* root : containers)
					if (isInside(
						root,
						event.mouseButton.x,
						event.mouseButton.y))
					{
						selected = root;

						break;
					}
			}

			switch (event.type)
			{
			// case ...:
			case sf::Event::MouseMoved:
			case sf::Event::MouseWheelScrolled:
				for (MENU::RootBase* const root : containers)
				{
					root->onEvent(event);
				}

				break;
			default:
				selected->onEvent(event);

				break;
			}
		}

		virtual void onLogic(
			const sf::Time time)
		{
			for (MENU::RootBase* const root : containers)
			{
				root->onLogic(time);
			}
		}

		virtual void onDraw()
		{
			for (MENU::RootBase* const root : containers)
			{
				root->onDraw();
			}
		}

	protected:
		bool isInside(
			MENU::RootBase* const root,
			const float mouse_x,
			const float mouse_y)
		{
			const float
				x = DEVICE::Interface::getScreen()->getWindow()->getSize().x * root->getView()->getViewport().left,
				y = DEVICE::Interface::getScreen()->getWindow()->getSize().y * root->getView()->getViewport().top,
				width = DEVICE::Interface::getScreen()->getWindow()->getSize().x * root->getView()->getViewport().width,
				height = DEVICE::Interface::getScreen()->getWindow()->getSize().y * root->getView()->getViewport().height;

			return
				mouse_x > x	&& mouse_x < x + width &&
				mouse_y > y	&& mouse_y < y + height;
		}

		Menu::RootBase* selected;

		void addRoot(
			Menu::RootBase* const root)
		{
			containers.push_back(root);
			
			static int i = 0;
			if (++i == 1)
				selected = root;
		}

		const std::vector<Menu::RootBase*>& getContainers() const
		{
			return containers;
		}

	private:
		std::vector<Menu::RootBase*> containers;
	};
}