#pragma once

#include <source\menu\MenuBase.h>

class SetupHostMenu : public MenuBase
{
public:
    using MenuBase::show;

    SetupHostMenu(
        tgui::Gui* userInterface);
private:
    tgui::Label::Ptr makeDescriptionLabel();
    tgui::TextBox::Ptr makeDescriptionTextBox();

    tgui::Label::Ptr makePasswordLabel();
    tgui::TextBox::Ptr makePasswordTextBox();

    tgui::Label::Ptr makePortLabel();
    tgui::TextBox::Ptr makePortTextBox();

    tgui::Label::Ptr makeWorldWidthLabel();
    tgui::TextBox::Ptr makeWorldWidthTextBox();

    tgui::Label::Ptr makeWorldHeightLabel();
    tgui::TextBox::Ptr makeWorldHeightTextBox();

    tgui::Label::Ptr makeBeginLengthLabel();
    tgui::TextBox::Ptr makeBeginLengthTextBox();

    tgui::Label::Ptr makeLengthPFoodLabel();
    tgui::TextBox::Ptr makeLengthPFoodTextBox();

    tgui::Label::Ptr makeFoodCountLabel();
    tgui::TextBox::Ptr makeFoodCountTextBox();

    tgui::Label::Ptr makeMaxPlayerLabel();
    tgui::TextBox::Ptr makeMaxPlayerTextBox();

    tgui::Button::Ptr makeDefaultButton();
    tgui::Button::Ptr makeBackButton();
};

