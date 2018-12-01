#include <Client/source/device/DeviceInterface.h>

#include <Client/source/logger/Logger.h>

bool handleInputError();
bool handleNetworkError();
bool handleSceneError();
bool handleScreenError();
bool handleResourceError();

bool (*handleError[(int) DEVICE::InitError::_Length])() =
{	// Sorted and para with InitError
	handleInputError,
	handleNetworkError,
	handleSceneError,
	handleScreenError,
	handleResourceError
};

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
#ifdef _DEBUG
	Log::Output::Add(Log::Output::CONSOLE_OUT);
#else
	Log::AddOutput(Log::OUTPUT_FILE);
	Log::SetOutputFilePath(L"logs.txt");
#endif
Retry:
	while (true)
	{
		Device::InitError result;

		{
			Log::Section section(L"Initializing Game");
			result = DEVICE::Interface::Initialize();
		}

		if (result == Device::InitError::Invalid)
		{
			break;
		}

		if (!handleError[(int) result]())
		{
			return 0;
		}
		else
		{
			Log::Information(L"Restarting Game...");
		}
	}

	Log::Information(L"Game successfully initialized");

	int result;
	try
	{
		result = DEVICE::Interface::Start();
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

bool handleInputError()
{
	if (MessageBoxW(
		NULL,
		L"Do you want to reset Input settings and retry?",
		L"Input Initialization Error",
		MB_YESNO)
		== IDNO)
	{
		return false;
	}

	// Delete settings

	return true;
}

bool handleNetworkError()
{
	// maybe offline mode?

	return true;
}

bool handleSceneError()
{
	// No Idea how to fix

	return false;
}

bool handleScreenError()
{
	if (MessageBoxW(
			NULL,
			L"Do you want to reset Graphic settings and retry?",
			L"Graphic Initialization Error",
			MB_YESNO)
		== IDNO)
	{
		return false;
	}

	// Delete settings

	return true;
}

bool handleResourceError()
{
	if (MessageBoxW(
		NULL,
		L"Do you want to reset All settings and retry?",
		L"Resource Initialization Error",
		MB_YESNO)
		== IDNO)
	{
		return false;
	}

	// Delete settings

	return true;
}
