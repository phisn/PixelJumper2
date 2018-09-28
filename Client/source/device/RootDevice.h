#pragma once

#include "InputDevice.h"
#include "WindowDevice.h"

#include <Client/source/scene/BootScene.h>
#include <Client/source/scene/SceneBase.h>
#include <Client/source/scene/SceneHandler.h>

#include <SFML/Main.hpp>

#ifndef DEVICE
#define DEVICE ::Device
#endif

namespace Device
{
	namespace Root
	{
		bool initialize();
		int start();

		void onTypeChanged(
			Scene::Type lastType,
			Scene::Type currentType);
	}

	class Root
	{
	public:
		bool initialize();
		int start();

		void onTypeChanged(
			Scene::Type lastType,
			Scene::Type currentType);
	private:
	};
}
