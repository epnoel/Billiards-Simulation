#pragma once
#include <SFML/Graphics.hpp>
#include "StereographicProjection.h"

#define PI 3.14159265f



class Ball {

public:
    Ball();

    void setUpBall(sf::Vector2f position, float mass, sf::Image* ballImage, float ballSize, int ballNumber);

    void updateImpulse();

    void drawBall();

    bool ballCollision(Ball ball1);

    bool mouseInsideCircle(int mouseX, int mouseY);

    void setBallNumber(int ballNumber);

public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;

    sf::Vector2f oldVelocity = { 0.f, 0.f };

    sf::Vector3f angularPosition;
    sf::Vector3f angularVelocity;
    sf::Vector3f angularAcceleration;

    sf::Vector2f impulse;
    sf::Vector2f angularMomentum;
    float mass;
    float radius;
    float inertia;
    float squareSize;
    float torque;
    float fNet;

    bool aimMode = false;
    float lineSlope = 0.f;
    float timeLeft;

    int ballNumber = 0;

    sf::Vector2f frictionCoefficient;

    sf::Vector3i color = { 255, 255, 255 };
    sf::CircleShape shape;
    sf::Image* ballImage;
    sf::Image projectionImage;
    sf::Texture* ballTexture;
    sf::Vector3f rotAngles = { PI / 2.f,-PI / 2.f,PI / 2.f };
    sf::Vector3f returnAngles = { 0.f,0.f,0.f };


};