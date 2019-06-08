#pragma once

#include <Client/source/framework/Animation.h>
#include <Client/source/framework/AsyncAnimation.h>
#include <Client/source/framework/ResourceContext.h>

#include <Client/source/scene/MainSceneBase.h>
#include <Client/source/scene/SubSceneBase.h>

#include <deque>
#include <stack>
#include <vector>

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

namespace Framework
{
	/*
	
		TODO: new feature -> asynchronous scenes
	
	*/

	class Context
	{
		static std::deque<Context*> contextStack;
		static enum class InternalTask
		{
			Empty,

			LoadScene,
			PopScene,

			Fallback,

			LoadContext,
			PopContext

		} currentTask;

		static bool isValidTask(const InternalTask task)
		{
			return (int) currentTask < (int) task;
		}

	public:
		static const std::deque<Context*>& GetStack()
		{
			return contextStack;
		}

		template <typename T, class... Args>
		static bool Push(Args... args)
		{
			Context* const newContext = new Context(new T(args...));

			// TODO: prepush and pop? (to fix resource problem [no on stack])

			if (!isValidTask(InternalTask::LoadContext) ||
				!newContext->internInit())
			{
				return false;
			}

			currentTask = InternalTask::LoadContext;

			if (!contextStack.empty())
			{
				contextStack.back()->hide();
			}

			contextStack.push_back(newContext);

			return true;
		}

		static bool Pop()
		{
			if (!isValidTask(InternalTask::PopContext))
			{
				return false;
			}

			currentTask = InternalTask::PopContext;

			return true;
		}

		static bool PushScene(Scene::SubBase* const scene)
		{
			if (!isValidTask(InternalTask::LoadScene) || !scene->onCreate())
			{
				return false;
			}

			currentTask = InternalTask::LoadScene;

			contextStack.back()->subScenes.push_back(scene);

			return true;
		}

		static bool PopScene()
		{
			std::deque<Scene::SubBase*>& scenes = contextStack.back()->subScenes;
			if (scenes.size() == 0 || !isValidTask(InternalTask::PopScene))
			{
				return false;
			}

			currentTask = InternalTask::PopScene;

			return true;
		}

		static void ProcessTask()
		{
			switch (currentTask)
			{
			case InternalTask::Empty:
				return;
			case InternalTask::Fallback:
				contextStack.back()->fallback();

				break;
			case InternalTask::LoadContext:
				contextStack.back()->externInit();

				break;
			case InternalTask::LoadScene:
				break;
			case InternalTask::PopContext:
				Log::Information(L"Poping");

				contextStack.back()->cleanup();
				contextStack.pop_back();

				if (contextStack.empty())
				{
					// TODO: Shutdown
				}
				else
				{
					contextStack.back()->show();
				}

				break;
			case InternalTask::PopScene:
				contextStack.back()->popScene();

				break;
			}

			currentTask = InternalTask::Empty;
		}

	private:
		Context(
			Scene::MainSceneBase* mainScene)
			:
			scene(mainScene)
		{
		}

		/*
			
			internal initialize, should allocate resources
				and setup internal objects. it can fail 
				but not call directly or indirectly any
				framework methods
			external initialize, should create new subscenes
				or interact with other scenes. it can NOT fail
				but can call framework methods
		*/

		bool internInit() const
		{
			return scene->onCreate();
		}

		void externInit() const
		{
			scene->initialize();
		}

		void hide()
		{
			fallback();
			scene->onHide();
		}

		void show() const
		{
			scene->onShow();
		}

		void cleanup()
		{
			fallback();

			if (scene)
			{
				scene->onRemove();
				delete scene;
			}
		}

		void fallback()
		{
			while (!subScenes.empty())
			{
				popScene();
			}
		}

		bool pushScene(Scene::SubBase* scene)
		{
			if (!scene->onCreate())
			{
				return false;
			}

			subScenes.push_back(scene);

			return true;
		}

		void popScene()
		{
			subScenes.back()->onRemove();
			delete subScenes.back();

			subScenes.pop_back();
			scene->onScenePopped(subScenes.size());
		}

	public:
		void draw() const
		{
			if (!subScenes.empty())
			{
				subScenes.back()->onDraw();
			}

			if (scene->isRunning())
			{
				scene->onDraw();
			}
		}

		void Event(const sf::Event event) const
		{
			if (!subScenes.empty())
			{
				subScenes.back()->onEvent(event);

				Log::Information(L"Sub Scene");
			}

			Log::Information(std::to_wstring(scene->isRunning()));

			if (scene->isRunning())
			{
				scene->onEvent(event);
			}
		}

		void update(const sf::Time time) const
		{
			if (!subScenes.empty())
			{
				subScenes.back()->onLogic(time);
			}

			if (scene->isRunning())
			{
				scene->onLogic(time);
			}
		}

		ResourceContext* getResource()
		{
			return &resourceContext;
		}

	private:
		ResourceContext resourceContext;

		std::deque<Scene::SubBase*> subScenes;
		Scene::MainSceneBase* const scene;
	};
}