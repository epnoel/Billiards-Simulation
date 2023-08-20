#pragma once

#include <SFML/Graphics.hpp>
#include "Ball.h"

class CuePowerBar {

public:
    CuePowerBar(Ball mainBall, sf::Vector2f normalCoords, float distanceFactor, float distanceMoved, sf::Vector2f cuePosition, sf::Vector2f cueSize, bool moveTheCue,
                float maxSpeed);

    void moveCue(float ballMouseDistance, float distanceFactor, sf::Vector2f cuePosition, sf::Vector2f cueSize);

    sf::Vector2f returnSpeedVector();

public:
    Ball mainBall;
    sf::Vector2i mousePos;
    float distanceFactor;
    float ballMouseDistance;
    float distanceMoved;
    float cueFactor = 0.25f;
    float maxSpeed;

    sf::Vector2f normalCoords;

    sf::RectangleShape cue;
    sf::RectangleShape cueDistanceMoved;
};