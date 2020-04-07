#pragma once

#include <Client/source/menu/MenuBackground.h>
#include <Client/source/menu/MenuButtonMaterial.h>
#include <Client/source/menu/MenuButtonWithLabel.h>
#include <Client/source/menu/MenuRowContainer.h>
#include <Client/source/menu/MenuTextBox.h>

#include <Client/source/scene/MenuBaseScene.h>

namespace Scene
{
	/*
	+------------------------------------+
	|       +-----------+        +-----+ |
	| IP:   |           |  Port: |     | |
	|       +-----------+        +-----+ |
	| +--------------+  +--------------+ |
	| |    Cancel    |  |     Join     | |
	| +--------------+  +--------------+ |
	+------------------------------------+
	*/

	class GameJoinMenu
		:
		public Menu::MenuRootBase
	{
	public:
		GameJoinMenu()
			:
			outerContainer(Menu::CommonControlDirection::Vertical),
			buttonContainer(Menu::CommonControlDirection::Horizontal),
			inputContainer(Menu::CommonControlDirection::Horizontal)
		{
		}

		bool build() override
		{
			// viewPort = { 0.3, 0.3, 0.4, 0.4 };
			virtualSize = sf::Vector2f{ 500, 250 };
			viewPortCenteredByVirtualSize({ 0.2f, 0.2f, 0.2f, 0.2f });
			outerContainer.innerOffset = { 10, 10, 10, 10 };

			constructChilds();

//			inputContainer.sizePreferred = { 200, 150 };
//			buttonContainer.sizePreferred = { 200, 150 };

			ipLabel.text = L"IP:";
			portLabel.text = L"Port:";

			cancelButton.labelDependentSize = { 0.2f, 0.2f };

			cancelButton.labelOffset = { 7.f, 10.f };
			cancelButton.value.text = L"Cancel";

			joinButton.value.text = L"Join";

			portTextBox.text = L"8295";

			joinButton.buttonPressedEvent.addListener(
				[]()
				{
					Log::Information(L"Join pressed");
				});

			cancelButton.buttonPressedEvent.addListener(
				[]()
				{
					Framework::Context::Pop();
				});

			return true;
		}

		void onDraw(sf::RenderTarget* const target) const override
		{
			MenuRootBase::onDraw(target);
		}

	private:
		void constructChilds();

		Menu::Background background;

		Menu::RowContainer outerContainer;
		Menu::RowContainer buttonContainer;
		Menu::RowContainer inputContainer;

		Menu::Label ipLabel, portLabel;
		Menu::TextBox ipTextBox, portTextBox;

		Menu::ButtonWithLabel<Menu::ButtonMaterial::DefaultStaticRectangle> cancelButton;
		Menu::ButtonWithLabel<Menu::ButtonMaterial::DefaultStaticRectangle> joinButton;
	};

	class GameJoinScene
		:
		public MenuBaseScene
	{
	public:
		bool onCreate() override
		{
			addRoot<GameJoinMenu>();
			return MenuBaseScene::onCreate();
		}
	};
}
