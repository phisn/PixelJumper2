#include "FrameworkCore.h"
#include "ResourceCore/ResourceInterface.h"

#include <deque>

namespace
{
	struct ChildContainer
	{
		Framework::Scene* scene;
		bool haltParent;
	};

	struct SceneContainer
	{
		Framework::Scene* scene;
		std::vector<ChildContainer> childs;
	};

	std::vector<SceneContainer> scenes;

	bool running = false;
	
	enum FrameworkCommand
	{
		Empty,

		LoadChild,
		UnloadChild,
		Fallback,

		Load,
		Unload

	} currentCommand;

	sf::Font defaultFont;
}

namespace Framework
{
	bool IsValidCommand(const FrameworkCommand command)
	{
		return running && currentCommand < command;
	}

	void ClearChildScenes();
	void ProcessFrameworkCommand();

	bool Core::Initialize()
	{
		running = true;

		Resource::ResourceTypeDefinition type;

		type.name = L"font";
		type.directory = L"static";
		type.extension = L"ttf";

		return defaultFont.loadFromFile(Resource::Interface::GetResourcePath(L"font", type).string());
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

		if (scenes.back().childs.size() > 0)
		{
			scenes.back().childs.back().scene->onEvent(event);
		}
		else
		{
			scenes.back().scene->onEvent(event);
		}
	}

	void Core::ProcessLogic(const sf::Time time)
	{
		ProcessFrameworkCommand();

		if (scenes.empty())
		{
			return;
		}

		if (scenes.back().childs.size() > 0)
		{
			ChildContainer& child = scenes.back().childs.back();
			child.scene->onLogic(time);
			
			if (child.haltParent)
				return;
		}

		scenes.back().scene->onLogic(time);
	}

	void Core::ProcessDraw(sf::RenderTarget* const target)
	{
		if (scenes.empty())
		{
			return;
		}

		if (scenes.back().childs.size() > 0)
		{
			ChildContainer& child = scenes.back().childs.back();
			child.scene->onDraw(target);

			if (child.haltParent)
				return;
		}

		scenes.back().scene->onDraw(target);
	}

	void Core::PushScene(Scene* const scene)
	{
		if (IsValidCommand(Load))
		{
			currentCommand = Load;

			if (scenes.size() > 0)
			{
				if (scenes.back().childs.size() > 0)
				{
					scenes.back().childs.back().scene->onHide();

					if (!scenes.back().childs.back().haltParent)
						scenes.back().scene->onHide();
				}
				else
				{
					scenes.back().scene->onHide();
				}
			}

			scenes.emplace_back().scene = scene;
		}
	}

	void Core::PopScene()
	{
		if (scenes.size() > 0 && IsValidCommand(Unload))
		{
			currentCommand = Unload;
		}
	}

	void Core::PushChildScene(
		Scene* scene,
		bool haltParent)
	{
		if (scenes.empty() || !IsValidCommand(LoadChild))
		{
			return;
		}

		currentCommand = LoadChild;

		if (haltParent && (scenes.back().childs.size() == 0 || !scenes.back().childs.back().haltParent))
		{
			scenes.back().scene->onHide();
		}
		
		// if any of the current childs already halts parent scene
		// then change haltParent to true
		if (!haltParent && scenes.back().childs.size() > 0)
		{
			for (ChildContainer& child : scenes.back().childs)
				if (child.haltParent)
				{
					haltParent = true;
					break;
				}
		}

		ChildContainer& child = scenes.back().childs.emplace_back();

		child.haltParent = haltParent;
		child.scene = scene;
	}

	void Core::PopChildScene()
	{
		if (scenes.back().childs.size() > 0 && IsValidCommand(UnloadChild))
		{
			currentCommand = UnloadChild;
		}
	}

	void Core::FallbackChildScene()
	{
		if (scenes.back().childs.size() > 0 && IsValidCommand(Fallback))
		{
			currentCommand = Fallback;
		}
	}

	void ClearChildScenes()
	{
		for (ChildContainer& child : scenes.back().childs)
			delete child.scene;

		scenes.back().childs.clear();
	}

	void ProcessFrameworkCommand()
	{
		while (currentCommand != Empty)
		{
			FrameworkCommand temp = currentCommand;
			currentCommand = Empty;

			switch (temp)
			{
			case LoadChild:
				scenes.back().childs.back().scene->initialize();

				break;
			case UnloadChild:
			{
				bool wasHaltParent = scenes.back().childs.back().haltParent;

				delete scenes.back().childs.back().scene;
				scenes.back().childs.pop_back();

				if ((scenes.back().childs.empty() || !scenes.back().childs.back().haltParent ) && wasHaltParent)
				{
					scenes.back().scene->onShow();
				}

				break;
			}
			case Fallback:
			{
				bool wasHaltParent = scenes.back().childs.back().haltParent;

				ClearChildScenes();

				if (wasHaltParent)
				{
					scenes.back().scene->onShow();
				}

				break;
			}
			case Load:
				scenes.back().scene->initialize();

				break;
			case Unload:
				ClearChildScenes();

				delete scenes.back().scene;
				scenes.pop_back();

				if (scenes.empty())
				{
					running = false;
				}
				else
				{
					scenes.back().scene->onShow();
				}

				break;
			}
		}
	}

	const sf::Font& GetFont()
	{
		return defaultFont;
	}
}
