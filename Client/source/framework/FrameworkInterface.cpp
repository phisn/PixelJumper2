#include "FrameworkInterface.h"

#include <Client/source/framework/Context.h>

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
		Context* context)
	{
		if (isValid(Task::LoadContext))
		{
			return false;
		}

		if (!context->quickInitialize())
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
		Scene::SubBase* scene)
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
		if (isValid(Order::LoadScene))
		{
			return false;
		}

		currentOrder = Order::PopScene;

		return true;
	}

	void Interface::Fallback()
	{
		contextStack.top()->fallback();
	}

	void Interface::PushAsyncAnimation(
		AsyncAnimation* animation)
	{
		contextStack.top()->pushAsyncAnimation(animation);
	}

	void Interface::PushSequentialAnimation(
		Animation* animation)
	{
		contextStack.top()->pushSequentialAnimation(animation);
	}

	void Interface::ResetAnimations()
	{
		contextStack.top()->removeAnimations();
	}


	void Interface::OnDraw()
	{
		contextStack.top()->onDraw();
	}

	void Interface::OnEvent(
		const sf::Event event)
	{
		contextStack.top()->onEvent(event);
	}

	void Interface::OnUpdate(
		const sf::Time time)
	{
		contextStack.top()->onUpdate(time);
	}

	void Interface::DoOrders()
	{
		if (currentOrder == Order::Empty)
		{
			return;
		}

		switch (currentOrder)
		{
		case Order::Fallback:
			contextStack.top()->fallback();

			break;
		case Order::PopScene:
			contextStack.top()->popScene();

			break;
		case Order::PopContext:
			// delete or reuse?
			contextStack.top()->cleanup();
			contextStack.pop();

			if (contextStack.empty())
			{
				shutdown();
			}
			else
			{
				contextStack.top()->onShow();
			}

			break;
		case Order::LoadContext:
			contextStack.top()->deepInitialize();

			break;
		}

		currentOrder = Order::Empty;
	}
	
	void Interface::Shutdown()
	{
		running = false;
	}

	bool Interface::IsRunning()
	{
		return running;
	}
}
