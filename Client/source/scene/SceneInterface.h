#pragma once

#include <stack>

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#ifndef SCENE
#define SCENE ::Scene
#endif

namespace Scene
{
	class Context;

	class Animation;
	class AsyncAnimation;

	namespace Interface
	{
		enum class Order
		{ // Order by importance
			Empty,

			LoadScene,	// Load already pushed scene (prevent more than one pushed)
			PopScene,

			Fallback,	// Removes all subscenes (Menu done)

			PopContext,
			LoadContext
		};

		bool pushContext(
			Context* context);
		bool popContext();
		
		bool pushScene(
			Scene::SubBase* scene);
		bool popScene();

		void fallback();

		void pushAsyncAnimation(
			AsyncAnimation* animation);
		void pushSequentialAnimation(
			Animation* animation);

		void resetAnimations();

		void onDraw();
		void onEvent(
			const sf::Event event);
		void onUpdate(
			const sf::Time time);
		void doOrders();
	}
}
