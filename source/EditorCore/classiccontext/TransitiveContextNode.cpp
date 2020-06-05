#include "TransitiveContextNode.h"
#include "TaskManager.h"
#include "task/CreateTransitiveTask.h"

void Editor::ClassicContext::TransitivePopup::onContent()
{
	if (ImGui::Button("Remove", ImVec2{ MaxPopupWidth, 0 }))
	{
		TaskManager::Instance()->pushTask(
			new RemoveTransitiveTask(node->getID())
		);
		closeWindow();
	}
}
