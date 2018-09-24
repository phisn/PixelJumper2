#pragma once

#include <TGUI\TGUI.hpp>
#include <vector>

#define MIDDLE_SIZE(p) (p / 100.f)
#define MIDDLE_POSI(p) ((1.f - MIDDLE_SIZE(p)) / 2.f)

class MenuBase
{
public:
    MenuBase(
        tgui::Gui* userInterface);

    void show();
    void addWidget(
        tgui::Widget::Ptr widget);

    tgui::Layout getWindowWidth();
    tgui::Layout getWindowHeight();
protected:
    tgui::Gui* userInterface;

    tgui::Layout windowWidth;
    tgui::Layout windowHeight;
    
    std::vector<tgui::Widget::Ptr> widgets;
};