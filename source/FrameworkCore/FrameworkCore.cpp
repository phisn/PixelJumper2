#include "FrameworkCore.h"

#include <deque>

namespace
{
	std::deque<Framework::Scene*> scenes;
	std::deque<Framework::Scene*> temporaryScenes;

	bool running = false;
	bool haltmain = false;

	enum FrameworkCommand
	{
		Empty,

		LoadTemporary,
		UnloadTemporary,
		Fallback,

		Load,
		Unload

	} currentCommand;
}

namespace Framework
{
	bool IsValidCommand(const FrameworkCommand command)
	{
		return running && currentCommand < command;
	}

	void ClearTemporaryScenes();
	void ProcessFrameworkCommand();

	bool Core::Initialize()
	{
		running = true;
		return running;
	}

	void Core::Uninitialize()
	{
	}

	bool Core::IsRunning()
	{
		return running;
	}

	void Core::Shutdown()
	{
		running = false;
	}

	void Core::ProcessEvent(const sf::Event event)
	{
		if (scenes.empty())
		{
			return;
		}

		if (temporaryScenes.size() > 0)
		{
			temporaryScenes.back()->onEvent(event);
		}
		else
		{
			scenes.back()->onEvent(event);
		}
	}

	void Core::ProcessLogic(const sf::Time time)
	{
		ProcessFrameworkCommand();

		if (scenes.empty())
		{
			return;
		}

		if (temporaryScenes.size() > 0)
		{
			temporaryScenes.back()->onLogic(time);

			if (haltmain)
				return;
		}

		scenes.back()->onLogic(time);
	}

	void Core::ProcessDraw(sf::RenderTarget* const target)
	{
		if (scenes.empty())
		{
			return;
		}

		if (temporaryScenes.size() > 0)
		{
			temporaryScenes.back()->onDraw(target);

			if (haltmain)
				return;
		}

		scenes.back()->onDraw(target);
	}

	bool Core::PushScene(Scene* const scene)
	{
		if (!IsValidCommand(Load) || !scene->onCreate())
		{
			return false;
		}

		currentCommand = Load;
		if (scenes.size() > 0)
		{
			scenes.back()->onHide();
		}

		scenes.push_back(scene);
		return true;
	}

	void Core::PopScene()
	{
		if (scenes.size() > 0 && IsValidCommand(Unload))
		{
			currentCommand = Unload;
		}
	}

	bool Core::PushTemporaryScene(
		Scene* const scene,
		const bool haltMainScene)
	{
		if (scenes.empty() ||
			!IsValidCommand(LoadTemporary) ||
			!scene->onCreate())
		{
			return false;
		}

		currentCommand = LoadTemporary;
		if (haltMainScene)
		{
			haltmain = true;
			scenes.back()->onHide();
		}

		temporaryScenes.push_back(scene);
		return true;
	}

	void Core::PopTemporaryScene()
	{
		if (temporaryScenes.size() > 0 && IsValidCommand(UnloadTemporary))
		{
			currentCommand = UnloadTemporary;
		}
	}

	void Core::FallbackTemporaryScene()
	{
		if (temporaryScenes.size() > 0 && IsValidCommand(Fallback))
		{
			currentCommand = Fallback;
		}
	}

	void ClearTemporaryScenes()
	{
		for (Scene* const scene : temporaryScenes)
			delete scene;

		temporaryScenes.clear();
	}

	void ProcessFrameworkCommand()
	{
		switch (currentCommand)
		{
		case LoadTemporary:
			temporaryScenes.back()->initialize();

			break;
		case UnloadTemporary:
			delete temporaryScenes.back();
			temporaryScenes.pop_back();

			if (temporaryScenes.empty() && haltmain)
			{
				haltmain = false;
				scenes.back()->onShow();
			}

			break;
		case Fallback:
			ClearTemporaryScenes();

			if (haltmain)
			{
				haltmain = false;
				temporaryScenes.back()->onShow();
			}

			break;
		case Load:
			scenes.back()->initialize();

			break;
		case Unload:
			ClearTemporaryScenes();
			haltmain = false;

			delete scenes.back();
			scenes.pop_back();

			if (scenes.empty())
			{
				running = false;
			}
			else
			{
				scenes.back()->onShow();
			}

			break;
		}
	}
}
