#include <source\scene\setup\SetupHostMenu.h>
#include <source\scene\setup\SetupMenuContainer.h>

SetupHostMenu::SetupHostMenu(
    tgui::Gui* userInterface)
    :
    MenuBase(userInterface)
{
    addWidget(makeDescriptionLabel());
    addWidget(makeDescriptionTextBox());

    addWidget(makePasswordLabel());
    addWidget(makePasswordTextBox());

    addWidget(makePortLabel());
    addWidget(makePortTextBox());

    addWidget(makeWorldWidthLabel());
    addWidget(makeWorldWidthTextBox());

    addWidget(makeWorldHeightLabel());
    addWidget(makeWorldHeightTextBox());

    addWidget(makeBeginLengthLabel());
    addWidget(makeBeginLengthTextBox());

    addWidget(makeLengthPFoodLabel());
    addWidget(makeLengthPFoodTextBox());

    addWidget(makeFoodCountLabel());
    addWidget(makeFoodCountTextBox());

    addWidget(makeMaxPlayerLabel());
    addWidget(makeMaxPlayerTextBox());

    addWidget(makeDefaultButton());
    addWidget(makeBackButton());
}


tgui::Label::Ptr SetupHostMenu::makeDescriptionLabel()
{
    tgui::Label::Ptr label = tgui::Label::create("Descritpion");

    label->setTextSize(windowWidth.getValue() * 0.02);
    label->setPosition(
        windowWidth * 0.1f,
        windowHeight * 0.1f);

    return label;
}

tgui::TextBox::Ptr SetupHostMenu::makeDescriptionTextBox()
{
    tgui::TextBox::Ptr textBox = tgui::TextBox::create();

    textBox->setPosition(
        windowWidth * 0.2f,
        windowHeight * 0.1f);
    textBox->setSize(
        windowWidth * 0.1f, 
        windowHeight * 0.02f);

    return textBox;
}

tgui::Label::Ptr SetupHostMenu::makePasswordLabel()
{
    tgui::Label::Ptr label = tgui::Label::create("Password");

    label->setTextSize(1);
    label->setPosition(
        0,
        0);

    return label;
}

tgui::TextBox::Ptr SetupHostMenu::makePasswordTextBox()
{
    tgui::TextBox::Ptr textBox = tgui::TextBox::create();

    textBox->setPosition(0, 0);
    textBox->setSize(0, 0);

    return textBox;
}

tgui::Label::Ptr SetupHostMenu::makePortLabel()
{
    tgui::Label::Ptr label = tgui::Label::create("Port");

    label->setTextSize(1);
    label->setPosition(
        0,
        0);

    return label;
}

tgui::TextBox::Ptr SetupHostMenu::makePortTextBox()
{
    tgui::TextBox::Ptr textBox = tgui::TextBox::create();

    textBox->setPosition(0, 0);
    textBox->setSize(0, 0);

    return textBox;
}

tgui::Label::Ptr SetupHostMenu::makeWorldWidthLabel()
{
    tgui::Label::Ptr label = tgui::Label::create("World width");

    label->setTextSize(1);
    label->setPosition(
        0,
        0);

    return label;
}

tgui::TextBox::Ptr SetupHostMenu::makeWorldWidthTextBox()
{
    tgui::TextBox::Ptr textBox = tgui::TextBox::create();

    textBox->setPosition(0, 0);
    textBox->setSize(0, 0);

    return textBox;
}

tgui::Label::Ptr SetupHostMenu::makeWorldHeightLabel()
{
    tgui::Label::Ptr label = tgui::Label::create("World height");

    label->setTextSize(1);
    label->setPosition(
        0,
        0);

    return label;
}

tgui::TextBox::Ptr SetupHostMenu::makeWorldHeightTextBox()
{
    tgui::TextBox::Ptr textBox = tgui::TextBox::create();

    textBox->setPosition(0, 0);
    textBox->setSize(0, 0);

    return textBox;
}

tgui::Label::Ptr SetupHostMenu::makeBeginLengthLabel()
{
    tgui::Label::Ptr label = tgui::Label::create("Begin length");

    label->setTextSize(1);
    label->setPosition(
        0,
        0);

    return label;
}

tgui::TextBox::Ptr SetupHostMenu::makeBeginLengthTextBox()
{
    tgui::TextBox::Ptr textBox = tgui::TextBox::create();

    textBox->setPosition(0, 0);
    textBox->setSize(0, 0);

    return textBox;
}

tgui::Label::Ptr SetupHostMenu::makeLengthPFoodLabel()
{
    tgui::Label::Ptr label = tgui::Label::create("Length per food");

    label->setTextSize(1);
    label->setPosition(
        0,
        0);

    return label;
}

tgui::TextBox::Ptr SetupHostMenu::makeLengthPFoodTextBox()
{
    tgui::TextBox::Ptr textBox = tgui::TextBox::create();

    textBox->setPosition(0, 0);
    textBox->setSize(0, 0);

    return textBox;
}

tgui::Label::Ptr SetupHostMenu::makeFoodCountLabel()
{
    tgui::Label::Ptr label = tgui::Label::create("Food count");

    label->setTextSize(1);
    label->setPosition(
        0,
        0);

    return label;
}

tgui::TextBox::Ptr SetupHostMenu::makeFoodCountTextBox()
{
    tgui::TextBox::Ptr textBox = tgui::TextBox::create();

    textBox->setPosition(0, 0);
    textBox->setSize(0, 0);

    return textBox;
}

tgui::Label::Ptr SetupHostMenu::makeMaxPlayerLabel()
{
    tgui::Label::Ptr label = tgui::Label::create("Max Players");

    label->setTextSize(1);
    label->setPosition(
        0,
        0);

    return label;
}

tgui::TextBox::Ptr SetupHostMenu::makeMaxPlayerTextBox()
{
    tgui::TextBox::Ptr textBox = tgui::TextBox::create();

    textBox->setPosition(0, 0);
    textBox->setSize(0, 0);

    return textBox;
}

tgui::Button::Ptr SetupHostMenu::makeDefaultButton()
{
    tgui::Button::Ptr button = tgui::Button::create("Default");

    button->setPosition(
        windowWidth * MIDDLE_POSI(40.f),
        windowHeight * 0.71f);
    button->setSize(
        windowWidth * MIDDLE_SIZE(40.f),
        windowHeight * 0.12f);
    button->connect("pressed", [this]()
    {
        SetupMenu::online->show();
    });

    return button;
}

tgui::Button::Ptr SetupHostMenu::makeBackButton()
{
    tgui::Button::Ptr button = tgui::Button::create("Back");

    button->setPosition(
        windowWidth * MIDDLE_POSI(40.f),
        windowHeight * 0.84f);
    button->setSize(
        windowWidth * MIDDLE_SIZE(40.f),
        windowHeight * 0.15f);
    button->connect("pressed", [this]()
    {
        SetupMenu::online->show();
    });

    return button;
}
