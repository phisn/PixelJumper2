#include "TransitiveContextNode.h"
#include "TaskManager.h"
#include "task/CreateTransitiveTask.h"

void Editor::ClassicContext::TransitivePopup::onContent()
{
	if (ImGui::MenuItem("Remove"))
	{
		TaskManager::Instance()->pushTask(
			new RemoveTransitiveTask(node->getID())
		);
		closeWindow();
	}
}
