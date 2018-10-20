#pragma once

#include <Client/source/scene/Animation.h>
#include <Client/source/scene/AsyncAnimation.h>
#include <Client/source/scene/MainSceneBase.h>
#include <Client/source/scene/SubSceneBase.h>

#include <stack>
#include <vector>

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

namespace Scene
{
	class Context
	{
	public:
		template <typename T, class... Args>
		static Context* create(Args&&... args)
		{
			return new Context(
				new T(args)
			);
		}

		Context(
			MainBase* mainScene)
			:
			mainScene(mainScene)
		{
		}

		_Success_(return == true)
		bool quickInitialize();
		void deepInitialize();

		void onHide();
		void onShow();
		void cleanup();

		void fallback();

		void onDraw() const;
		void onEvent(
			const sf::Event event);
		void onUpdate(
			const sf::Time time);

		bool pushScene(
			SubBase* scene);
		void popScene();

		void pushAsyncAnimation(
			AsyncAnimation* animation);
		void pushSequentialAnimation(
			Animation* animation);
		void removeAnimations();

	private:
		std::stack<SubBase*> subScenes;
		MainBase* mainScene;

		void updateSeqAnimation(
			const sf::Time time);

		bool seqAnimRunning = false;
		Animation* sequentialAnimation;

		void updateAsyncAnimations(
			const sf::Time time);

		std::vector<AsyncAnimation*> asyncAnimations;
	};

}