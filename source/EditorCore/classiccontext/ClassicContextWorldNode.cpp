#include "ClassicContextWorldNode.h"
#include "TaskManager.h"
#include "task/CreateWorldTask.h"

void Editor::ClassicContext::WorldPopup::onContent()
{
	if (ImGui::Button("Connect to", ImVec2{ MaxPopupWidth, 0 }))
	{
		access->beginLink(worldNode);
		closeWindow();
	}

	if (ImGui::Button("Remove", ImVec2{ MaxPopupWidth, 0 }))
	{
		TaskManager::Instance()->pushTask(
			new RemoveWorldTask(worldNode->getID())
		);
		closeWindow();
	}
}
