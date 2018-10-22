#include "Context.h"

namespace Scene
{
	bool Scene::Context::quickInitialize()
	{
		return mainScene->onCreate();
	}
	
	void Context::deepInitialize()
	{
		mainScene->initialize();
	}
	
	void Context::onHide()
	{
		delete sequentialAnimation;

		for (AsyncAnimation* animation : asyncAnimations)
		{
			delete animation;
		}

		fallback(); // clear subscenes

		mainScene->onHide();
	}
	
	void Context::onShow()
	{
		mainScene->onShow();
	}
	
	void Context::cleanup()
	{
		if (sequentialAnimation)
		{
			delete sequentialAnimation;
		}

		for (AsyncAnimation* animation : asyncAnimations)
		{
			delete animation;
		}

		fallback(); // clear subscenes

		if (mainScene)
		{
			mainScene->onRemove();
			delete mainScene;
		}
	}
	
	void Context::fallback()
	{
		while (!subScenes.empty())
		{
			popScene();
		}
	}
	
	void Context::onDraw() const
	{
		if (!subScenes.empty())
		{
			subScenes.top()->onDraw();
		}

		if (mainScene->isRunning())
		{
			mainScene->onDraw();
		}

		if (seqAnimRunning)
		{
			sequentialAnimation->onDraw();
		}
		else
		{
			for (const AsyncAnimation* animation : asyncAnimations)
			{
				animation->onDraw();
			}
		}
	}
	
	void Context::onEvent(
		const sf::Event event)
	{
		if (mainScene->isRunning())
		{
			mainScene->onEvent(event);
		}

		if (!subScenes.empty())
		{
			subScenes.top()->onEvent(event);
		}
	}
	
	void Context::onUpdate(
		const sf::Time time)
	{
		if (seqAnimRunning)
		{
			updateSeqAnimation(time);
		}
		else
		{
			updateAsyncAnimations(time);
		}

		if (mainScene->isRunning())
		{
			mainScene->onLogic(time);
		}

		if (!subScenes.empty())
		{
			subScenes.top()->onLogic(time);
		}
	}
	
	bool Context::pushScene(
		SubBase* scene)
	{
		if (!scene->onCreate())
		{
			return false;
		}

		subScenes.push(scene);

		return true;
	}
	
	void Context::popScene()
	{
		subScenes.top()->onRemove();
		delete subScenes.top();

		subScenes.pop();
	}
	
	void Context::pushAsyncAnimation(
		AsyncAnimation* animation)
	{
		asyncAnimations.push_back(animation);
	}
	
	void Context::pushSequentialAnimation(
		Animation* animation)
	{
		seqAnimRunning = true;
		sequentialAnimation = animation;
	}
	
	void Context::removeAnimations()
	{
		asyncAnimations.clear();
	}
	
	void Context::updateSeqAnimation(
		const sf::Time time)
	{
		seqAnimRunning = !sequentialAnimation->onUpdate(time);

		// On Sequential Animation finished
	}
	
	void Context::updateAsyncAnimations(
		const sf::Time time)
	{
		int i = 0;

		while (i < asyncAnimations.size())
			if (!asyncAnimations[i]->onUpdate(time))
			{
				delete asyncAnimations[i];

				if (i + 1 != asyncAnimations.size())
				{
					asyncAnimations[i] = asyncAnimations.back();
				}

				asyncAnimations.pop_back();
			}
			else
			{
				++i;
			}
	}
}
