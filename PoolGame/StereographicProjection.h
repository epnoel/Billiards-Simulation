#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

#define PI 3.14159265f



class StereographicProjection {

public:
    StereographicProjection(sf::Image baseImage, sf::Image& editImage, int circleRadius, sf::Vector3f& returnAngles,
        sf::Vector3f originalAngles = { 0.f,0.f,0.f }, sf::Vector2f velocityVector = { 0.f,0.f }, float rotationFactor = 0.05f);

    bool convertToStereographicImage(sf::Image originalImage, sf::Image& editImage, int circleRadius, sf::Vector3f& returnMidPointAngles,
        sf::Vector3f originalAngles = { 0.f,0.f,0.f }, sf::Vector3f angles = { 0.f,0.f,0.f });

    sf::Vector2f convertToRegularCoords(sf::Vector2f normalizedVal, int height, int width);

    bool rollBallAlongVelocityVector(sf::Image originalImage, sf::Image& editImage, int circleRadius, sf::Vector3f& returnAngles,
        sf::Vector3f originalAngles = { 0.f,0.f,0.f }, sf::Vector2f velocityVector = { 0.f,0.f }, float rotationFactor = 0.005f);
};
