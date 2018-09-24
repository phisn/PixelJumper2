#include "World.h"

World::World()
{
    Player::world = this;
    size = sf::Vector2i(40, 30);

    Input* input = new Input();

    input->changeKey(Key_Type::Up, sf::Keyboard::W);
    input->changeKey(Key_Type::Left, sf::Keyboard::A);
    input->changeKey(Key_Type::Down, sf::Keyboard::S);
    input->changeKey(Key_Type::Right, sf::Keyboard::D);

    LocalPlayer* localPlayer = new LocalPlayer(input);

    localPlayers.push_back(localPlayer);
}

World::~World()
{
}
