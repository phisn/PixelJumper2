#pragma once

#include <functional>
#include <stack>
#include <vector>

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#ifndef SCENE
#define SCENE ::Scene
#endif

namespace Scene
{
	class SubSceneBase
	{
	public:
		virtual bool onCreate() = 0;
		virtual void onRemove() = 0;

		virtual void onEvent(
			sf::Event event) = 0;
		virtual void onLogic(
			sf::Time time) = 0;
		virtual void onDraw() = 0;
	};

	class MainSceneBase
	{
	public:
		virtual bool onCreate() = 0;
		virtual void onRemove() = 0;

		virtual void onShow() = 0;
		virtual void onHide() = 0;

		virtual void onEvent(
			sf::Event event) = 0;
		virtual void onLogic(
			sf::Time time) = 0;
		virtual void onDraw() = 0;

		bool isRunning() const
		{
			return running;
		}
	protected:
		bool running = true;
	};

	class Animation
	{
	public:
		bool update(
			const sf::Time time)
		{

		}

		void reset()
		{

		}

	private:
	};

	class AsyncAnimation
	{
	public:
		bool stopRepeating()
		{
			doesRepeat = false;
		}

		void setOnFinished(
			std::function<void()> onFinished)
		{
			this->onFinished = onFinished;
		}

		bool update(
			const sf::Time time)
		{
			if (animation->update(time))
			{
				if (doesRepeat)
				{
					animation->reset();
				}
				else
				{
					if (onFinished)
					{
						onFinished();
					}

					return false;
				}
			}

			return true;
		}

	private:
		Animation* animation;

		bool doesRepeat = false;
		std::function<void()> onFinished;
	};

	class Context
	{
	public:
		bool quickInitialize()
		{

		}

		void deepInitialize()
		{

		}

		void update(
			const sf::Time time)
		{
			if (seqAnimRunning)
			{
				updateSeqAnimation(time);

				return;
			}

			updateAsyncAnimations(time);

			if (mainScene->isRunning())
			{
				mainScene->onLogic(time);
			}

			subScenes.top()->onLogic(time);
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

	private:
		std::stack<SubSceneBase*> subScenes;
		MainSceneBase* mainScene;

		void updateSeqAnimation(
			const sf::Time time)
		{
			seqAnimRunning = !sequentialAnimation->update(time);

			// On Sequential Animation finished
		}

		bool seqAnimRunning = false;
		Animation* sequentialAnimation;

		void updateAsyncAnimations(
			const sf::Time time)
		{
			int i = 0;

			while (i < asyncAnimations.size())
				if (!asyncAnimations[i]->update(time))
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

	namespace Interface
	{
#define HAS_ENUM(var, val) ((int) var & (int) val)
#define SET_ENUM(var, val) ((int&) var|= (int) val)
		enum class Order
		{
			PopSubScene,
			LoadSubScene, // Load already pushed scene (prevent more than one pushed)

			PopContext,
			LoadContext
		};

		std::stack<Context*> contextStack;
		Order orders;

		bool pushContext(
			Context* context)
		{
			if (HAS_ENUM(orders, Order::LoadContext))
			{
				return false;
			}

			if (!context->quickInitialize())
			{
				return false;
			}

			contextStack.top()->onHide();
			SET_ENUM(orders, Order::LoadContext);
			contextStack.push(context);

			return true;
		}

		void popContext()
		{
			
		}

		void update(
			const sf::Time time)
		{

		}

		void doOrders()
		{
			if (HAS_ENUM(orders, Order::LoadContext))
			{
				contextStack.top()->deepInitialize();
			}
		}

		void draw()
		{

		}
	}
}
