#pragma once

#include <Client/source/scene/SceneBase.h>

#include <SFML/Graphics.hpp>

#ifndef DEVICE
#define DEVICE ::Device
#endif

namespace Device
{
	class Screen;
	class GlobalInput;

	namespace Interface
	{
		bool initialize();
		int start();

		Screen* getScreen();
		GlobalInput* getInput();

		void onSceneTypeChanged(
			SCENE::Type oldType,
			SCENE::Type newType);
	}
}
