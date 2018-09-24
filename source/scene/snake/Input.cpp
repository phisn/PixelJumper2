#include "Input.h"

Input::Input()
    :
    keyPairs((int) Key_Type::None - 1)
{
}


Input::~Input()
{
    
}

void Input::onKeyPressed(
    sf::Keyboard::Key key)
{
    Key_Type keyType = getType(key);
    if (keyType == Key_Type::None)
    {
        return;
    }

    keyQueue.push_front(keyType);

    //if (keyQueue.size() > bufferSize)
    {
   //     return keyQueue.pop_back();
    }
}

Key_Type&& Input::pullKey()
{
    if (keyQueue.size() > 0)
    {
        Key_Type key = keyQueue.back();
        keyQueue.pop_back();

        return std::move(key);
    }
    else
    {
        return std::move(Key_Type::None);
    }
}

Key_Type Input::getType(
    sf::Keyboard::Key key)
{
    for (int i = 0; i < (int) Key_Type::None - 1; ++i)
    {
        if (keyPairs[i] == key)
            return (Key_Type) i;
    }

    return Key_Type::None;
}

sf::Keyboard::Key Input::getKey(
    Key_Type keyType) const
{
    return keyPairs[(int) keyType];
}

void Input::changeKey(
    Key_Type keyType,
    sf::Keyboard::Key key)
{
    keyPairs[(int) keyType] = key;
}
