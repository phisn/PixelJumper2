#include <source/scene/setup/SetupHostMenu.h>
#include <source/scene/setup/SetupMenuContainer.h>

SetupHostMenu::SetupHostMenu(
    tgui::Gui* userInterface)
    :
    MenuBase(userInterface)
{
    tgui::Grid::Ptr outerGrid = tgui::Grid::create();
    tgui::Grid::Ptr grid = tgui::Grid::create();

    grid->addWidget(makeDescriptionGrid(), 0, 0);
    grid->addWidget(makePasswordGrid(), 0, 1);

    grid->addWidget(makePortGrid(), 1, 0, tgui::Borders(), tgui::Grid::Alignment::Left);

    outerGrid->addWidget(grid, 0, 0);
    outerGrid->addWidget(makeDefaultButton(), 1, 0);
    outerGrid->addWidget(makeBackButton(), 2, 0);

    outerGrid->setPosition(
        (windowWidth.getValue() / 2) - (outerGrid->getSize().x / 2),
        (windowHeight.getValue() / 2) - (outerGrid->getSize().y / 2));

    addWidget(outerGrid);
}

tgui::Grid::Ptr SetupHostMenu::makeDescriptionGrid()
{
    tgui::Grid::Ptr grid = tgui::Grid::create();

    tgui::Label::Ptr label = tgui::Label::create("Description: ");
    grid->addWidget(label, 0, 0);

    tgui::EditBox::Ptr textBox = tgui::EditBox::create();
    textBox->setInputValidator(".*");
    grid->addWidget(textBox, 0, 1);
    grid->setSize(
        windowWidth * 0.1f,
        windowHeight * 0.05f);

    return grid;
}

tgui::Grid::Ptr SetupHostMenu::makePasswordGrid()
{
    tgui::Grid::Ptr grid = tgui::Grid::create();

    tgui::Label::Ptr label = tgui::Label::create("Password: ");
    grid->addWidget(label, 0, 0);

    tgui::EditBox::Ptr textBox = tgui::EditBox::create();
    textBox->setInputValidator(".*");
    grid->addWidget(textBox, 0, 1);

    return grid;
}

tgui::Grid::Ptr SetupHostMenu::makePortGrid()
{
    tgui::Grid::Ptr grid = tgui::Grid::create();

    tgui::Label::Ptr label = tgui::Label::create("Port: ");
    grid->addWidget(label, 0, 0);
    
    tgui::EditBox::Ptr textBox = tgui::EditBox::create();
    textBox->setInputValidator("\\d{0,5}");
    grid->addWidget(textBox, 0, 1, tgui::Borders(), tgui::Grid::Alignment::Right);

    return grid;
}

tgui::Button::Ptr SetupHostMenu::makeDefaultButton()
{
    tgui::Button::Ptr button = tgui::Button::create("Default");

    button->setSize(
        windowWidth * MIDDLE_SIZE(40.f),
        windowHeight * 0.12f);
    button->connect("pressed", [this]()
    {

    });

    return button;
}

tgui::Button::Ptr SetupHostMenu::makeStartButton()
{
    tgui::Button::Ptr button = tgui::Button::create("Default");

    button->setSize(
        windowWidth * MIDDLE_SIZE(40.f),
        windowHeight * 0.12f);
    button->connect("pressed", [this]()
    {

    });

    return button;
}

tgui::Button::Ptr SetupHostMenu::makeBackButton()
{
    tgui::Button::Ptr button = tgui::Button::create("Back");

    button->setSize(
        windowWidth * MIDDLE_SIZE(40.f),
        windowHeight * 0.12f);
    button->connect("pressed", [this]()
    {
        SetupMenu::online->show();
    });

    return button;
}
