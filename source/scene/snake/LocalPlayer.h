#pragma once

#include "Player.h"

#include <SFML\Graphics.hpp>
#include <vector>

class LocalPlayer : public Player
{
public:
    LocalPlayer(
        Input* input);
    ~LocalPlayer();

    void updateView();
    void updateZoom(float factor);

    sf::View& getView()
    {
        return view;
    }

    Input* getInput() const
    {
        return input;
    }
private:
    void onSnakeMove();

    Input* input;
    sf::View view;
};

