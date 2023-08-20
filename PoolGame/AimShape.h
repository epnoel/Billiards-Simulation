#pragma once

#include <SFML/Graphics.hpp>
#include "Ball.h"
#include <vector>
#include <iostream>

class AimShape {

public:
    AimShape();

    AimShape(Ball ball1, std::vector<Ball> balls, sf::Vector2f normalizedBallMouseVector, sf::Vector2f screenSize, float addedLength, bool usingOtherWallLines = false,
        std::vector<std::vector<float>> contactLineValues = { });

    int getClosestBallIndex(std::vector<Ball> balls, sf::Vector2f normalizedBallMouseVector, Ball ball1);

    Ball getClosestBall(std::vector<Ball> balls, Ball ball1);

    Ball getCorrectBallAlongLine(std::vector<Ball> balls, sf::Vector2f normalizedBallMouseVector, sf::Vector2f screenSize, Ball ball1, bool usingOtherWallLines = false,
        std::vector<std::vector<float>> contactLineValues = { });


    sf::Vector2f calculateNormalLineEndPoints(Ball ball1, Ball ball2, float addedDistance, float cosTheta, sf::Vector2f normalCoords);

    sf::Vector2f calculateTangentLineEndPoints(Ball ball2, float addedDistance, float straightSlope, float sinTheta, sf::Vector2f tangentCoords);

    sf::Vector2f calculateAimLineEndPoints(Ball ball1, sf::Vector2f normalizedBallMouseVector, sf::Vector2f screenSize, bool usingOtherWallLines = false,
        std::vector<std::vector<float>> contactLineValues = { });

public:
    std::vector<sf::Vertex> aimLine;
    sf::CircleShape aimCircleProjection;
    std::vector<sf::Vertex> aimTangentLine;
    std::vector<sf::Vertex> aimNormalLine;

    Ball targetBall;

    double slope;
    double inverseSlope;
    bool circleOn = false;


};

