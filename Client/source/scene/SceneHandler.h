#pragma once

#include "SceneBase.h"

#include <functional>
#include <vector>

namespace Scene
{
	typedef std::function<void(Type, Type)> OnTypeChanged;

	namespace Handler
	{
		bool pushScene(
			Base* scene);
		bool popScene();

		void onDraw(
			::Device::Window* window);
		void onLogic(
			sf::Time time);
		void onEvent(
			sf::Event event);

		size_t getSceneSize();
		Type getCurrentType();

		void setOnTypeChanged(
			OnTypeChanged onTypeChanged);
	}
}
