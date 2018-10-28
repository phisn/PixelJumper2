#pragma once

#include <Client/source/scene/MainSceneBase.h>

namespace Scene
{
	class Editor
		:
		public MainBase
	{
	public:
		bool onCreate() override { }
		void onRemove() override
		{

		}

		void initialize() override
		{

		}

		void onShow() override { }
		void onHide() override { }
		
		void onEvent(const sf::Event event) override
		{

		}

		void onLogic(const sf::Time time) override
		{

		}

		void onDraw() override
		{
			
		}
	};
}
