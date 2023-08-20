#include "Ball.h"


Ball::Ball() {
    position = sf::Vector2f(0.f, 0.f);
    velocity = sf::Vector2f(0.f, 0.f);
    acceleration = sf::Vector2f(0.f, 0.f);

    angularPosition = sf::Vector3f(0.f, 0.f, 0.f);
    angularVelocity = sf::Vector3f(0.f, 0.f, 0.f);
    angularAcceleration = sf::Vector3f(0.f, 0.f, 0.f);

    impulse = sf::Vector2f(0.f, 0.f);
    mass = 1.f;
    radius = 50.f;
    inertia = (0.4f) * mass * powf(radius, 2);

    shape = sf::CircleShape(50.f);
    shape.setFillColor(sf::Color::Green);
    shape.setPosition(position.x, position.y);
    shape.setOrigin(50.f, 50.f);
}

void Ball::setUpBall(sf::Vector2f position, float mass, sf::Image* ballImage, float ballSize, int ballNumber)
{
    this->position = position;
    this->mass = mass;
    this->radius = ballSize / 2.f;
    updateImpulse();

    this->ballImage = new sf::Image;
    this->ballImage = ballImage;

    projectionImage.create(ballSize, ballSize, sf::Color::Black);
    StereographicProjection step1(*ballImage, projectionImage, ballSize / 2.f, returnAngles, rotAngles);
    ballTexture = new sf::Texture;
    ballTexture->loadFromImage(projectionImage);

    drawBall();
    shape.setTexture(ballTexture);
    shape.setTextureRect(sf::IntRect(0, 0, (int)ballSize, (int)ballSize));
    setBallNumber(ballNumber);

}

void Ball::updateImpulse() {
    impulse = { velocity.x * mass, velocity.y * mass };
}

void Ball::drawBall() {
    shape = sf::CircleShape(radius);
    shape.setOrigin(radius, radius);
    shape.setPosition(position);
}

bool Ball::ballCollision(Ball ball1) {
    float radius1 = ball1.radius;
    double distance = sqrt(pow((double) (ball1.position.x - position.x), 2) + pow( (double) (ball1.position.y - position.y), 2));

    if (distance <= (double) (radius1 + this->radius)) {

        return true;
    }

    return false;
}

bool Ball::mouseInsideCircle(int mouseX, int mouseY) {
    if (pow(mouseX - position.x, 2) + pow(mouseY - position.y, 2) <= pow(radius, 2)) {
        return true;
    }

    return false;
}

void Ball::setBallNumber(int ballNumber)
{
    this->ballNumber = ballNumber;
}