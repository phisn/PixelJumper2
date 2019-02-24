#include "FrameworkInterface.h"

#include <Client/source/framework/Context.h>

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ResourceDevice.h>

#include <stack>

namespace
{
	std::stack<Framework::Context*> contextStack;
	Framework::Interface::Task currentOrder
		= Framework::Interface::Task::Empty;

	bool isValid(const Framework::Interface::Task order)
	{
		return (int) currentOrder >= (int) order;
	}

	bool running = true;
}

namespace Framework
{
	bool Interface::PushContext(
		Context* const context)
	{
		if (isValid(Task::LoadContext))
		{
			return false;
		}

		if (!context->internalInitialize())
		{
			return false;
		}

		currentOrder = Task::LoadContext;

		if (!contextStack.empty())
		{
			contextStack.top()->onHide();
		}

		contextStack.push(context);

		return true;
	}

	bool Interface::PopContext()
	{
		if (isValid(Task::PopContext))
		{
			return false;
		}

		currentOrder = Task::PopContext;

		return true;
	}

	bool Interface::PushScene(
		Scene::SubBase* const scene)
	{
		if (isValid(Task::PopScene))
		{
			return false;
		}

		if (!contextStack.top()->pushScene(scene))
		{
			return false;
		}

		// currentOrder = Order::PopScene;

		return true;
	}

	bool Interface::PopScene()
	{
		if (isValid(Task::LoadScene))
		{
			return false;
		}

		currentOrder = Task::PopScene;

		return true;
	}

	void Interface::Fallback()
	{
		contextStack.top()->fallback();
	}

	void Interface::PushAsyncAnimation(
		AsyncAnimation* const animation)
	{
		contextStack.top()->pushAsyncAnimation(animation);
	}

	void Interface::PushSequentialAnimation(
		Animation* const animation)
	{
		contextStack.top()->pushSequentialAnimation(animation);
	}

	void Interface::ResetAnimations()
	{
		contextStack.top()->removeAnimations();
	}


	void Execution::OnDraw()
	{
		contextStack.top()->onDraw();
	}

	void Execution::OnEvent(
		const sf::Event event)
	{
		contextStack.top()->onEvent(event);
	}

	void Execution::OnUpdate(
		const sf::Time time)
	{
		contextStack.top()->onUpdate(time);
	}

	void Execution::DoTasks()
	{
		if (currentOrder == Interface::Task::Empty)
		{
			return;
		}

		switch (currentOrder)
		{
		case Interface::Task::Fallback:
			contextStack.top()->fallback();

			break;
		case Interface::Task::PopScene:
			contextStack.top()->popScene();

			break;
		case Interface::Task::PopContext:
			// delete or reuse?
			contextStack.top()->cleanup();
			contextStack.pop();

			if (contextStack.empty())
			{
				Shutdown();
			}
			else
			{
				contextStack.top()->onShow();
			}

			break;
		case Interface::Task::LoadContext:
			contextStack.top()->externalInitialize();

			break;
		}

		currentOrder = Interface::Task::Empty;
	}
	
	void Execution::Shutdown()
	{
		running = false;
	}

	bool Execution::IsRunning()
	{
		return running;
	}

	sf::MemoryInputStream Resource::Get(
		const RESOURCE::Static::ID type)
	{
		for (Framework::Context* context : contextStack._Get_container())
			// quick because of 'ResourceContext' cache
			if (context->getResource()->has(type))
			{
				return context->getResource()->get(type);
			}

		return contextStack.top()->getResource()->obtain(type);
	}
}
