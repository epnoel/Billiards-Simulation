#include "CuePowerBar.h"

CuePowerBar::CuePowerBar(Ball mainBall, sf::Vector2f normalCoords, float distanceFactor, float distanceMoved, sf::Vector2f cuePosition, sf::Vector2f cueSize, bool moveTheCue,
    float maxSpeed) 
{
    this->mainBall = mainBall;
    this->mousePos = mousePos;
    this->distanceFactor = distanceFactor;

    this->normalCoords = normalCoords;

    this->normalCoords.x *= (1.f);
    this->normalCoords.y *= (1.f);

    this->distanceMoved = distanceMoved;

    this->maxSpeed = maxSpeed;

    cue.setPosition(cuePosition);
    cue.setSize(cueSize);
    cue.setFillColor(sf::Color::White);

    cueDistanceMoved.setPosition(cuePosition);
    cueDistanceMoved.setFillColor(sf::Color::Red);

    if (moveTheCue) {
        moveCue(distanceMoved, distanceFactor, cuePosition, cueSize);
    }
}

void CuePowerBar::moveCue(float ballMouseDistance, float distanceFactor, sf::Vector2f cuePosition, sf::Vector2f cueSize) {
    float cueDistance = distanceFactor * ballMouseDistance * cueFactor;
    cue.setPosition({ cuePosition.x, cuePosition.y + cueDistance });
    cueDistanceMoved.setSize({ cueSize.x, cueSize.y + cueDistance });
}

sf::Vector2f CuePowerBar::returnSpeedVector() {
    
    float userMagnitude = sqrtf(powf(distanceMoved * distanceFactor * normalCoords.x, 2) + powf(distanceMoved * distanceFactor * normalCoords.y, 2));

    if (userMagnitude < maxSpeed)
    {
        return { distanceMoved * distanceFactor * normalCoords.x, distanceMoved * distanceFactor * normalCoords.y };
    }
    else {
        return { maxSpeed * normalCoords.x, maxSpeed * normalCoords.y };
    }

}