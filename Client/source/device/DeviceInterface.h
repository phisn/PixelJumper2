#pragma once

#include <Client/source/scene/SceneBase.h>

#include <SFML/Graphics.hpp>

#ifndef DEVICE
#define DEVICE ::Device
#endif

namespace Device
{
	class Screen;
	class Input;

	namespace Interface
	{
		bool initialize();
		int start();

		Screen* getScreen();
		Input* getInput();

		void onSceneTypeChanged(
			SCENE::Type oldType,
			SCENE::Type newType);
	}
}
