#pragma once

#include <SFML\Graphics.hpp>

#include <iostream>
#include <deque>
#include <unordered_map>

enum class Key_Type
{
    Down,
    Right,
    Left,
    Up,

    OpenMenu,

    None,
};

class Input
{
public:
    Input();
    ~Input();

    void onKeyPressed(
        sf::Keyboard::Key key);
    Key_Type&& pullKey();

    Key_Type getType(sf::Keyboard::Key key_code);
    sf::Keyboard::Key getKey(Key_Type key) const;
    void changeKey(Key_Type key, sf::Keyboard::Key key_code);

    void setBufferSize(
        int bufferSize)
    {
        this->bufferSize = bufferSize;
    }
private:
    int bufferSize;

    std::deque<Key_Type> keyQueue;
    std::vector<sf::Keyboard::Key> keyPairs;
};