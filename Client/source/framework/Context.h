#pragma once

#include <Client/source/framework/Animation.h>
#include <Client/source/framework/AsyncAnimation.h>
#include <Client/source/framework/ResourceContext.h>

#include <Client/source/scene/MainSceneBase.h>
#include <Client/source/scene/SubSceneBase.h>

#include <stack>
#include <vector>

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

namespace Framework
{
	class Context
	{
	public:
		template <typename T, class... Args>
		static Context* create(Args&&... args)
		{
			return new Context(
				new T(args...)
			);
		}

		Context(
			SCENE::MainBase* mainScene)
			:
			mainScene(mainScene)
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

		_Success_(return == true)
		bool internalInitialize();
		void externalInitialize();

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
			SCENE::SubBase* scene);
		void popScene();

		void pushAsyncAnimation(
			AsyncAnimation* animation);
		void pushSequentialAnimation(
			Animation* animation);
		void removeAnimations();

		ResourceContext* getResource()
		{
			return &resourceContext;
		}

	private:
		ResourceContext resourceContext;

		std::stack<SCENE::SubBase*> subScenes;
		SCENE::MainBase* mainScene;

		void updateSeqAnimation(
			const sf::Time time);
		bool seqAnimRunning = false;
		Animation* sequentialAnimation;

		void updateAsyncAnimations(
			const sf::Time time);

		std::vector<AsyncAnimation*> asyncAnimations;
	};

}