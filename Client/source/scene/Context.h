#pragma once

#include <Client/source/scene/Animation.h>
#include <Client/source/scene/AsyncAnimation.h>
#include <Client/source/scene/MainSceneBase.h>
#include <Client/source/scene/SubSceneBase.h>

#include <stack>
#include <vector>

namespace Scene
{
	class Context
	{
	public:
		Context(MainBase* mainScene)
			:
			mainScene(mainScene)
		{
		}

		bool quickInitialize()
		{
			return mainScene->onCreate();
		}

		void deepInitialize()
		{
			mainScene->onHide();
		}

		void onUpdate(
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

			if (!subScenes.top())
			{
				subScenes.top()->onLogic(time);
			}
		}

		void cleanup()
		{
			// ...
		}

		void onHide()
		{
			delete sequentialAnimation;

			for (AsyncAnimation* animation : asyncAnimations)
			{
				delete animation;
			}

			mainScene->onHide();
		}

		void removeAnimations()
		{
			asyncAnimations.clear();
		}

		bool pushScene(
			SubBase* scene)
		{
			if (!scene->onCreate())
			{
				return false;
			}

			subScenes.push(scene);

			return true;
		}

		void onEvent(
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

	private:
		std::stack<SubBase*> subScenes;
		MainBase* mainScene;

		void updateSeqAnimation(
			const sf::Time time)
		{
			seqAnimRunning = !sequentialAnimation->onUpdate(time);

			// On Sequential Animation finished
		}

		bool seqAnimRunning = false;
		Animation* sequentialAnimation;

		void updateAsyncAnimations(
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

		std::vector<AsyncAnimation*> asyncAnimations;
	};

}