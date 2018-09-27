#pragma once

#include <Client/source/scene/DebugMenuScene.h>
#include <Client/source/scene/SceneBase.h>
#include <Client/source/scene/SceneHandler.h>

namespace Scene
{
	class Boot
		:
		public Base
	{
	public:
		Boot()
			:
			Base(Type::Boot)
		{
		}
		
	private:
		bool onCreate() override;
		bool onRemove() override;
		
		void onShow() override;
		void onHide() override;
		
		void onEvent(
			sf::Event event) override;
		void onLogic(
			sf::Time time) override;
		void onDraw(
			::Device::Window* window) override;
	};
}

