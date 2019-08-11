#include "FrameworkInterface.h"

#include <Client/source/framework/Context.h>
#include <Client/source/device/CoreDevice.h>

#include <stack>

namespace
{
	bool running = true;
}

namespace Framework
{
	void Interface::Draw(sf::RenderTarget* const target)
	{
		Context::GetStack().back()->draw(target);
	}

	void Interface::Event(const sf::Event event)
	{
		Context::GetStack().back()->Event(event);
	}

	void Interface::Update(const sf::Time time)
	{
		Context::GetStack().back()->update(time);
	}

	void Interface::ProcessTask()
	{
		Context::ProcessTask();
	}

	bool Interface::IsRunning()
	{
		return running;
	}

	void Interface::Shutdown()
	{
		running = false;
	}

}
