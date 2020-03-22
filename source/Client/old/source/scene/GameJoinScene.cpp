#include "GameJoinScene.h"

namespace Scene
{
	void GameJoinMenu::constructChilds()
	{
		addChild(&background);
		addChild(&outerContainer);

		outerContainer.addElement(&buttonContainer);
		outerContainer.addElement(&inputContainer);

		inputContainer.addElement(&ipLabel);
		inputContainer.addElement(&ipTextBox);
		inputContainer.addElement(&portLabel);
		inputContainer.addElement(&portTextBox);

		buttonContainer.addElement(&cancelButton);
		buttonContainer.addElement(&joinButton);

	}
}
