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

};

