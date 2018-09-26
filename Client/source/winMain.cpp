#include "scene/DebugMenuScene.h"
#include <Client/source/device/RootDevice.h>

int WINAPI wWinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	PWSTR pCmdLine, 
	int nCmdShow)
{
Retry:
	Device::Root root;
	if (root.initialize())
	{
		const int result = MessageBoxW(
			NULL,
			L"Failed to initialize Root Device\nDo you want to reset graphic settings?",
			L"Error",
			MB_YESNO);

		if (result == IDYES)
		{
			// Reset settings

			goto Retry;
		}

		return 1;
	}

	return root.start();
}
