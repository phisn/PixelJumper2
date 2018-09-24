#pragma once

#include "LocalPlayer.h"
#include "RemotePlayer.h"

#include <SFML\Graphics.hpp>

#include <vector>

class World
{
public:
    World();
    ~World();

    const std::vector<LocalPlayer*>& getLocalPlayer() const
    {
        return localPlayers;
    }

    const std::vector<RemotePlayer*>& getRemotePlayer() const
    {
        return remotePlayes;
    }

    const sf::Vector2i getSize() const
    {
        return size;
    }
private:
    sf::Vector2i size;

    std::vector<LocalPlayer*> localPlayers;
    std::vector<RemotePlayer*> remotePlayes;

    std::vector<Player*> players;
};

