#pragma once

#include <source\menu\MenuBase.h>
#include <source\scene\settings\SettingsScene.h>

class SetupMainMenu : private MenuBase
{
public:
    using MenuBase::show;

    SetupMainMenu(
        tgui::Gui* userInterface);
private:
    tgui::Label::Ptr&& makeLabelButton();
    tgui::Button::Ptr&& makeOnlineButton();
    tgui::Button::Ptr&& makeOfflineButton();
    tgui::Button::Ptr&& makeSettingsButton();
    tgui::Button::Ptr&& makeExitButton();
};

