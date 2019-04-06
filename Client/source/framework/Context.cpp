#include "Context.h"

namespace Framework
{
	std::deque<Context*> Context::contextStack;
	Context::InternalTask Context::currentTask = Context::InternalTask::Empty;
}
