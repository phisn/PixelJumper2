#pragma once

#include <source\menu\MenuBase.h>

#include <TGUI\TGUI.hpp>

class SetupOnlineMenu : private MenuBase
{
public:
    using MenuBase::show;

    SetupOnlineMenu(
        tgui::Gui* userInterface);
private:
    tgui::Button::Ptr makeHostButton();
    tgui::Button::Ptr makeJoinButton();
    tgui::Button::Ptr makeBackButton();
};

