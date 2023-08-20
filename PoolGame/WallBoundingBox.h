#pragma once

#include <SFML/Graphics.hpp>

#include <vector>

class WallBoundingBox {

public:
    WallBoundingBox() {

    }

    std::vector<std::vector<sf::Vector2f>> boxes;
};

