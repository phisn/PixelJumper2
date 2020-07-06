#include "ClassicContextWorldNode.h"
#include "TaskManager.h"
#include "task/CreateWorldTask.h"

void Editor::ClassicContext::WorldPopup::onContent()
{
	if (ImGui::MenuItem("Connect to"))
	{
		access->beginLink(worldNode);
		closeWindow();
	}

	if (ImGui::MenuItem("Remove"))
	{
		TaskManager::Instance()->pushTask(
			new RemoveWorldTask(worldNode->getID())
		);
		closeWindow();
	}
}
