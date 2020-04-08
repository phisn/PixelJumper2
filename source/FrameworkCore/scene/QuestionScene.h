#pragma once

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/imgui/QuestionModalWindow.h"

namespace Scene
{
	class QuestionScene
		:
		public Framework::Scene
	{
	public:
		QuestionScene(
			Framework::QuestionModalWindow::Callback callback,
			std::string question)
		{
			window.open(callback, question);
		}

		void onEvent(const sf::Event event) override
		{
		}

		void onLogic(const sf::Time time) override
		{
			if (!window.process())
			{
				Framework::Core::PopScene();
			}
		}
		
		void onDraw(sf::RenderTarget* const target) override
		{
		}

	private:
		Framework::QuestionModalWindow window;
	};
}
