#pragma once

#include <SFML/Graphics.hpp>

#include <deque>

namespace Editor
{
	class Task
	{
	public:
		virtual void undo() = 0;
		virtual void redo() = 0;
	};

	class EditorWindow
	{
	public:
		virtual void onProcess() = 0;
		virtual void onEvent(sf::Event event) = 0;

	protected:
		std::deque<Task*> tasks;
	};
}
