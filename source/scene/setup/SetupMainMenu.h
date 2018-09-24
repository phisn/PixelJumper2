#pragma once

#include <source\menu\MenuBase.h>
#include <source\scene\settings\SettingsScene.h>

class SetupMainMenu : public MenuBase
{
public:
    SetupMainMenu(
        tgui::Gui* userInterface);
private:
    tgui::Label::Ptr makeLabel();
    tgui::Button::Ptr makeOnlineButton();
    tgui::Button::Ptr makeOfflineButton();
    tgui::Button::Ptr makeSettingsButton();
    tgui::Button::Ptr makeExitButton();
};

