#pragma once

#include "FrameworkCore/FrameworkCore.h"

#include <imgui/imgui.h>
#include <imgui-sfml/imgui-SFML.h>

namespace Scene
{
	class EditorScene
		:
		public Framework::Scene
	{
	public:
		bool onCreate() override
		{
		}
		
		void onEvent(const sf::Event event) override
		{
		}
		
		void onLogic(const sf::Time time) override
		{
		}
		
		void onDraw(sf::RenderTarget* const target) override
		{
		}

	private:

	};
}
