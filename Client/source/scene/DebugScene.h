#pragma once

#include <Client/source/scene/SceneBase.h>

namespace Scene
{
	class Debug
		:
		public Base
	{
	public:
		Debug()
			:
			Base(false, Scene::Type::Windows)
		{
		}

		bool onCreate() override;
		void onRemove() override;

		void onShow() override;
		void onHide() override;

		void onEvent(
			sf::Event event) override;
		void onLogic(
			sf::Time time) override;
		void onDraw() override;
	private:
		void pushSinglePlayer(int playerCount);
	};
}
