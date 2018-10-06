#include <Client/source/device/DeviceInterface.h>

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
#endif
{
Retry:
	if (!DEVICE::Interface::initialize())
	{
		if (MessageBoxW(
				NULL,
				L"Failed to initialize Root Device\nDo you want to reset graphic settings?",
				L"Error",
				MB_YESNO) 
			== IDYES)
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
