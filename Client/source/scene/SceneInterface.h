#pragma once

#include <Client/source/scene/SceneBase.h>

#include <SFML/Graphics.hpp>

#ifndef SCENE
#define SCENE ::Scene
#endif

namespace Scene
{
	namespace Interface
	{
		bool initialize();

		void fallback(
			FallbackType type);

		bool pushScene(
			Base* scene);
		void popScene();

		void onDraw();
		void onLogic(
			sf::Time time);
		void onEvent(
			sf::Event event);

		size_t getSceneSize();
		Type getCurrentType();
	}
}
