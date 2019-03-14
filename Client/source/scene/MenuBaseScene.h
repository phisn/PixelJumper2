#pragma once

#include <Client/source/menu/RootBase.h>

#include <Client/source/scene/MainSceneBase.h>

#include <vector>

namespace Scene
{
	class MenuBase
		:
		public MainSceneBase
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

					(float) event.mouseButton.x,
					(float) event.mouseButton.y)
				)
			{
				for (MENU::RootBase* root : containers)
					if (isInside(
							root,
							(float) event.mouseButton.x,
							(float) event.mouseButton.y)
						)
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
				x = Device::Screen::GetWindow()->getSize().x * root->getView()->getViewport().left,
				y = Device::Screen::GetWindow()->getSize().y * root->getView()->getViewport().top,
				width =		Device::Screen::GetWindow()->getSize().x * root->getView()->getViewport().width,
				height =	Device::Screen::GetWindow()->getSize().y * root->getView()->getViewport().height;

			return
				mouse_x > x	&& mouse_x < x + width &&
				mouse_y > y	&& mouse_y < y + height;
		}

		Menu::RootBase* selected;

		template <class Root, class... Args>
		Root* addRoot(Args... arguments)
		{
			Root* root = new Root(arguments...);
			containers.push_back(selected = root);
			return root;
		}

		const std::vector<Menu::RootBase*>& getContainers() const
		{
			return containers;
		}

	private:
		std::vector<Menu::RootBase*> containers;
	};
}