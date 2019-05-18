#pragma once

#include <Client/source/menu/MenuRootBase.h>

#include <Client/source/scene/MainSceneBase.h>

#include <vector>

namespace Scene
{
	class MenuBaseScene
		:
		public MainSceneBase
	{
	public:
		virtual bool onCreate() override
		{
			for (Menu::MenuRootBase* const root : roots)
				if (!root->build())
				{
					return false;
				}

			return true;
		}

		virtual void onRemove() override
		{
			
		}

		virtual void initialize() override
		{
			for (Menu::MenuRootBase* const root : roots)
				root->initialize();
		}

		virtual void onShow() override
		{
		}

		virtual void onHide() override
		{
		}

		virtual void onEvent(const sf::Event event) override
		{
			switch (event.type)
			{
			case sf::Event::KeyPressed:
				onKeyPressed(event);

				break;
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Button::Left)
				{
					selectStrongByPosition(
					{
						(float) event.mouseButton.x,
						(float) event.mouseButton.y
					});
				}

				break;
			case sf::Event::MouseMoved:
				selectWeakByPosition(
				{
					(float) event.mouseButton.x,
					(float) event.mouseButton.y
				});

				break;
			}

			if (weakSelected != strongSelected)
				switch (event.type)
				{
				case sf::Event::MouseMoved:
					weakSelected->onEvent(event);

					break;
				}

			strongSelected->onEvent(event);
		}

		virtual void onLogic(const sf::Time time) override
		{
			for (Menu::MenuRootBase* const root : roots)
				root->onLogic(time);
		}

		virtual void onDraw() override
		{
			for (Menu::MenuRootBase* const root : roots)
				root->draw();
		}

	protected:
		typedef std::vector<Menu::MenuRootBase*> RootContainer;

		template <class Root, class... Args>
		Root* addRoot(Args... arguments)
		{
			static_assert(
				std::is_base_of_v<Menu::MenuRootBase, Root>, 
				"Invalid root added to menu");

			Root* root = new Root(arguments...);
			containers.push_back(
				weakSelected = strongSelected = root
			);
			return root;
		}

		const RootContainer& getRoots() const
		{
			return roots;
		}

		Menu::MenuRootBase* getWeakSelected() const
		{
			return weakSelected;
		}

		Menu::MenuRootBase* getStrongSelected() const
		{
			return strongSelected;
		}

	private:
		void onKeyPressed(const sf::Event event)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Key::Tab:
			case sf::Keyboard::Key::Enter:
			case sf::Keyboard::Key::Up:
			case sf::Keyboard::Key::Left:
			case sf::Keyboard::Key::Down:
			case sf::Keyboard::Key::Right:
				break;
			}
		}

		void selectStrongByPosition(const sf::Vector2f position)
		{
			if (isInsideRoot(strongSelected, position))
			{
				return;
			}

			if (isInsideRoot(weakSelected, position))
			{
				strongSelected = weakSelected;
				return;
			}

			if (Menu::MenuRootBase* const root = getRootByPosition(position); root != NULL)
			{
				strongSelected = root;
			}
		}

		void selectWeakByPosition(const sf::Vector2f position)
		{
			if (isInsideRoot(weakSelected, position))
			{
				return;
			}

			if (Menu::MenuRootBase* const root = getRootByPosition(position); root != NULL)
			{
				weakSelected = root;
			}
		}

		Menu::MenuRootBase* getRootByPosition(const sf::Vector2f position) const
		{
			for (Menu::MenuRootBase* const root : roots)
				if (isInsideRoot(root, position))
				{
					return root;
				}

			return NULL;
		}

		bool isInsideRoot(
			const Menu::MenuRootBase* const root, 
			const sf::Vector2f position) const
		{
			return Menu::MenuRootBase::ConvertPortToReal(root->viewPort).contains(position);
		}

		Menu::MenuRootBase* weakSelected;
		Menu::MenuRootBase* strongSelected;

		RootContainer roots;
	};

	/*class SubMenuBase
		:
		public SubBase
	{
	public:
	};*/

	/*
	class _MenuBase
		:
		public MainSceneBase
	{
	public:
		virtual ~_MenuBase() { };

		virtual void onEvent(
			const sf::Event event)
		{
			/*
			
				Create strong select and weak select
				Strong: MouseButtonPressed -> for important events
				Weak:   MouseMoved -> for visual events
			
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
	};*/
}