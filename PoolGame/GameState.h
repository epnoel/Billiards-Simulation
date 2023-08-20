#include <vector>
#include "Ball.h"
#include <sstream>
#include <iostream>
#include <unordered_map>

#pragma once

class GameState {

public:
    GameState();

    GameState(std::vector<Ball> balls);

    std::string encryptBallInfo(Ball ball, int ballNumber);

    void printGameState();

    std::string getGameStateFullString();

    std::vector<float> decryptBallInfo(std::string encryptedBallInfo);

    void loadGameStateFromEncryption(std::string encryptedGameString, std::vector<Ball>& balls, std::unordered_map<int, std::pair<std::string, sf::Image*>> ballFileNames, float mass, float ballSize);

    void loadGameStateFromGameState(GameState gameState, std::vector<Ball>& balls, std::unordered_map<int, std::pair<std::string, sf::Image*>> ballFileNames, float mass, float ballSize);

    sf::Vector2f generateIndividualRandomBallPosition(sf::Vector2f boundsX, sf::Vector2f boundsY);

    void generateRandomBallPositions(std::vector<Ball>& balls, sf::Vector2f xBound, sf::Vector2f yBound);

public:
    std::vector<std::string> encryptedGamePositions;

};

