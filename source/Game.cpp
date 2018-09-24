#include <source/Game.h>

Game* Game::game;

Game::Game()
{
    game = this;
    window = new Window();

    loadWinConfig();

    std::cout << window->getVideMode().width << " : " << window->getVideMode().height << std::endl;
}

Game::~Game()
{
    delete window;
}

void Game::run()
{
    sf::Clock clock;
    sf::Time time;
    sf::Event event;

    while (window->isAlive() && sceneStack.size() != 0)
    {
        while (window->renderWindow()->pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window->renderWindow()->close();

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window->renderWindow()->close();
                }

                break;
            }

            for (IScene* scene : sceneStack._Get_container())
            {
                scene->onEvent(event);
            }

            if (sceneStack.size() == 0)
            {
                Window::renderWindow()->close();
            }
        }

        time = clock.restart();
        for (IScene* scene : sceneStack._Get_container())
        {
            scene->onLogic(time);
        }

        window->renderWindow()->clear();

        // IMPORTANT: DRAW ON 'IS FRONT' ELSE ONLY ON SPECIAL
        for (IScene* scene : sceneStack._Get_container())
        {
            scene->onDraw();
        }

        window->renderWindow()->display();
    }

    while (sceneStack.size() > 0)
    {
        SceneHandler::instance()->popScene();
    }
}

void Game::loadWinConfig()
{
    window->setVideMode(
        window->getVideMode().getFullscreenModes()[0]);

    window->create();
}
