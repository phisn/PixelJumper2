#pragma once

#include <source/menu/MenuBase.h>

class SetupHostMenu : public MenuBase
{
public:
    SetupHostMenu(
        tgui::Gui* userInterface);

private:
    tgui::Grid::Ptr makeDescriptionGrid();
    tgui::Grid::Ptr makePasswordGrid();

    tgui::Grid::Ptr makePortGrid();
    tgui::Button::Ptr makeDefaultButton();
    tgui::Button::Ptr makeStartButton();
    tgui::Button::Ptr makeBackButton();

    tgui::TextBox::Ptr descriptionTextBox;
    tgui::TextBox::Ptr passwordTextBox;
    tgui::TextBox::Ptr portTextBox;
};

