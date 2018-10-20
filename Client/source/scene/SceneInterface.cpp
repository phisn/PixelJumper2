#include "SceneInterface.h"

#include <Client/source/scene/Context.h>

namespace
{
	std::stack<Scene::Context*> contextStack;
	Scene::Interface::Order currentOrder = Scene::Interface::Order::Empty;

	bool isValid(const Scene::Interface::Order order)
	{
		return (int) currentOrder >= (int) order;
	}

	bool running = true;
}

namespace Scene
{
	bool Interface::pushContext(
		Context* context)
	{
		if (isValid(Order::LoadContext))
		{
			return false;
		}

		if (!context->quickInitialize())
		{
			return false;
		}

		currentOrder = Order::LoadContext;

		if (!contextStack.empty())
		{
			contextStack.top()->onHide();
		}

		contextStack.push(context);

		return true;
	}

	bool Interface::popContext()
	{
		if (isValid(Order::PopContext))
		{
			return false;
		}

		currentOrder = Order::PopContext;

		return true;
	}

	bool Interface::pushScene(
		Scene::SubBase* scene)
	{
		if (isValid(Order::PopScene))
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

	bool Interface::popScene()
	{
		if (isValid(Order::LoadScene))
		{
			return false;
		}

		currentOrder = Order::PopScene;

		return true;
	}

	void Interface::fallback()
	{
		contextStack.top()->fallback();
	}

	void Interface::pushAsyncAnimation(
		AsyncAnimation* animation)
	{
		contextStack.top()->pushAsyncAnimation(animation);
	}

	void Interface::pushSequentialAnimation(
		Animation* animation)
	{
		contextStack.top()->pushSequentialAnimation(animation);
	}

	void Interface::resetAnimations()
	{
		contextStack.top()->removeAnimations();
	}


	void Interface::onDraw()
	{
		contextStack.top()->onDraw();
	}

	void Interface::onEvent(
		const sf::Event event)
	{
		contextStack.top()->onEvent(event);
	}

	void Interface::onUpdate(
		const sf::Time time)
	{
		contextStack.top()->onUpdate(time);
	}

	void Interface::doOrders()
	{
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

			if (!contextStack.empty())
			{
				contextStack.top()->onShow();
			}

			break;
		case Order::LoadContext:
			contextStack.top()->deepInitialize();

			break;
		}
	}
	
	void Interface::shutdown()
	{
		running = false;
	}

	bool Interface::isRunning()
	{
		return running;
	}
}
