#include <Client/source/device/DeviceInterface.h>
#include <Client/source/logger/Logger.h>

#include <Windows.h>

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

int main()
{
	Log::Output::Add(Log::Output::CONSOLE_OUT);

Retry:
	while (true)
	{
		const Device::InitError result = DEVICE::Interface::Initialize();

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

	return DEVICE::Interface::Start();
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
