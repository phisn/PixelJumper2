#include "GameJoinScene.h"

namespace Scene
{
	void GameJoinMenu::constructChilds()
	{
		addChild(&background);
		addChild(&outerContainer);

		outerContainer.addChild(&buttonContainer);
		outerContainer.addChild(&inputContainer);

		inputContainer.addChild(&ipLabel);
		inputContainer.addChild(&ipTextBox);
		inputContainer.addChild(&portLabel);
		inputContainer.addChild(&portTextBox);

		buttonContainer.addChild(&cancelButton);
		buttonContainer.addChild(&joinButton);

	}
}
