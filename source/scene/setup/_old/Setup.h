#include <source\Window.h>
#include <source\scene\setup\SetupMenuContainer.h>
#include <source\scene\IScene.h>

#include <TGUI\TGUI.hpp>

class Setup : public IScene
{
public:
    Setup();

    void onShutdown() override;
    void onEvent(sf::Event & event) override;
    void onLogic(sf::Time & time) override;
    void onDraw() override;

    static Setup* instance();
private:
    static Setup* setup;

    tgui::Gui* userInterface;
};
