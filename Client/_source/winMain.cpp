#include <Client/device/DeviceInterface.h>

#ifdef _WIN32
#include <Windows.h>
#ifdef _DEBUG
int main()
#else
int WINAPI wWinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	PWSTR pCmdLine, 
	int nCmdShow)
#endif
#else
#error Operating System supported
#endif
{
Retry:
	if (!DEVICE::Interface::initialize())
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

	int result;

	try
	{
		result = DEVICE::Interface::start();
	}
	catch (...)
	{
		MessageBox(
			NULL,
			L"Catched exception",
			L"Error",
			MB_OK);
	}

	return result;
}
