#include "Context.h"

#include <Client/source/framework/FrameworkInterface.h>

namespace Framework
{
	std::deque<Context*> Context::contextStack;
	Context::InternalTask Context::currentTask = Context::InternalTask::Empty;

	void Context::ProcessTask()
	{
		switch (currentTask)
		{
		case InternalTask::Empty:
			return;
		case InternalTask::Fallback:
			contextStack.back()->fallback();

			break;
		case InternalTask::LoadContext:
			contextStack.back()->externInit();

			break;
		case InternalTask::LoadScene:
			break;
		case InternalTask::PopContext:
			Log::Information(L"Poping");

			contextStack.back()->cleanup();
			contextStack.pop_back();

			if (contextStack.empty())
			{
				Framework::Interface::Shutdown();
			}
			else
			{
				contextStack.back()->show();
			}

			break;
		case InternalTask::PopScene:
			contextStack.back()->popScene();

			break;
		}

		currentTask = InternalTask::Empty;
	}
}
