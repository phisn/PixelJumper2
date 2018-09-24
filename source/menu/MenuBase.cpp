#include <source\menu\MenuBase.h>

MenuBase::MenuBase(
    tgui::Gui* userInterface)
    :
    userInterface(userInterface)
{
    windowWidth = tgui::bindWidth(*userInterface);
    windowHeight = tgui::bindHeight(*userInterface);
}

void MenuBase::show()
{
    userInterface->removeAllWidgets();

    for (tgui::Widget::Ptr& widget : widgets)
    {
        userInterface->add(widget);
    }
}

void MenuBase::addWidget(
    tgui::Widget::Ptr widget)
{
    widgets.push_back(widget);
}

tgui::Layout MenuBase::getWindowWidth()
{
    return windowWidth;
}

tgui::Layout MenuBase::getWindowHeight()
{
    return windowHeight;
}
