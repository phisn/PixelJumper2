#pragma once

#include "SceneBase.h"

#include <functional>
#include <vector>

namespace Scene
{
	namespace Handler
	{
		bool pushScene(
			Base* scene);
		bool popScene();

		void onDraw();
		void onLogic(
			sf::Time time);
		void onEvent(
			sf::Event event);

		size_t getSceneSize();
		Type getCurrentType();
	}
}
