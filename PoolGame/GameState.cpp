#include "GameState.h"

GameState::GameState() {

}

GameState::GameState(std::vector<Ball> balls) {

    //int number = 0;

    for (auto i : balls) {
        std::string encryption = encryptBallInfo(i, i.ballNumber);
        //number++;
        encryptedGamePositions.push_back(encryption);
    }
}

std::string GameState::encryptBallInfo(Ball ball, int ballNumber) {
    std::string encryptInfo = "";
    int xPos = (int)(ball.position.x * 1000.f);
    int yPos = (int)(ball.position.y * 1000.f);

    std::stringstream ss;
    ss << std::hex << ballNumber;

    encryptInfo += ss.str();
    encryptInfo += ".";

    std::stringstream ssX;
    ssX << std::hex << xPos;

    encryptInfo += ssX.str();
    encryptInfo += ".";

    std::stringstream ssY;
    ssY << std::hex << yPos;
    encryptInfo += ssY.str();

    return encryptInfo;
}

void GameState::printGameState() {
    for (int i = 0; i < encryptedGamePositions.size(); i++) {

        std::cout << encryptedGamePositions[i];

        if (i != encryptedGamePositions.size() - 1) {
            std::cout << ";";
        }
    }
}

std::string GameState::getGameStateFullString() {

    std::string someString = "";

    for (int i = 0; i < encryptedGamePositions.size(); i++) {

        someString += encryptedGamePositions[i];

        if (i != encryptedGamePositions.size() - 1) {
            someString += ";";
        }
    }

    // Debug
    std::cout << "someString = " << someString << std::endl;
    std::cout << "encryptedGamePositions.size() = " << encryptedGamePositions.size() << std::endl;

    return someString;
}

std::vector<float> GameState::decryptBallInfo(std::string encryptedBallInfo) {


    std::vector<std::string> info;
    std::string currentInfo = "";


    for (char& i : encryptedBallInfo) {

        if (i != '.') {
            currentInfo += i;
        }
        else {
            info.push_back(currentInfo);
            currentInfo.clear();
        }

    }

    info.push_back(currentInfo);

    float ballNum = (float)stoi(info[0], 0, 16);
    float xPos = ((float)stoi(info[1], 0, 16)) / 1000.f;
    float yPos = ((float)stoi(info[2], 0, 16)) / 1000.f;

    return { ballNum, xPos, yPos };

}

void GameState::loadGameStateFromEncryption(std::string encryptedGameString, std::vector<Ball>& balls, std::unordered_map<int, std::pair<std::string, sf::Image*>> ballFileNames,
                                            float mass, float ballSize) {

    std::string currentBallPos = "";
    encryptedGamePositions.clear();

    for (auto b : balls)
    {
        delete b.ballTexture;
        b.ballImage = nullptr;
        delete b.ballImage;
    }

    balls.clear();

    for (auto i : encryptedGameString) {
        if (i != ';') {
            currentBallPos += i;
        }
        else {
            std::vector<float> ballAndPos = decryptBallInfo(currentBallPos);
            encryptedGamePositions.push_back(currentBallPos);
            int ballAndPosVal = (int)ballAndPos[0];
            Ball theBall;
            //theBall.ballNumber = ballAndPosVal;
            theBall.position.x = ballAndPos[1];
            theBall.position.y = ballAndPos[2];
            //theBall.ballImage = ballFileNames[ballAndPosVal].second;

            theBall.setUpBall(theBall.position, mass, ballFileNames[ballAndPosVal].second, ballSize, ballAndPosVal);
            balls.push_back(theBall);

            currentBallPos = "";
        }
    }

    std::vector<float> ballAndPos = decryptBallInfo(currentBallPos);
    encryptedGamePositions.push_back(currentBallPos);
    int ballAndPosVal = (int)ballAndPos[0];
    Ball theBall;
    //theBall.ballNumber = ballAndPosVal;
    theBall.position.x = ballAndPos[1];
    theBall.position.y = ballAndPos[2];
    //theBall.ballImage = ballFileNames[ballAndPosVal].second;

    theBall.setUpBall(theBall.position, mass, ballFileNames[ballAndPosVal].second, ballSize, ballAndPosVal);
    balls.push_back(theBall);

    currentBallPos = "";

}

void GameState::loadGameStateFromGameState(GameState gameState, std::vector<Ball>& balls, std::unordered_map<int, std::pair<std::string, sf::Image*>> ballFileNames,
                                            float mass, float ballSize) 
{

    // Debug
    std::cout << "gameState.getGameStateFullString() = " << gameState.getGameStateFullString() << std::endl;

    loadGameStateFromEncryption(gameState.getGameStateFullString(), balls, ballFileNames, mass, ballSize);

}

sf::Vector2f GameState::generateIndividualRandomBallPosition(sf::Vector2f boundsX, sf::Vector2f boundsY) {

    srand(time(NULL) + rand());

    sf::Vector2f randPosition = { -1.f,-1.f };
    float diffX = boundsX.y - boundsX.x;
    float diffY = boundsY.y - boundsY.x;


    while (randPosition.x < boundsX.x || randPosition.x > boundsX.y) {


        randPosition.x = fmod(rand() + boundsX.x, diffX);

    }

    while (randPosition.y < boundsY.x || randPosition.y > boundsY.y) {
        randPosition.y = fmod(rand() + boundsY.x, diffY);
    }

    return randPosition;


}

void GameState::generateRandomBallPositions(std::vector<Ball>& balls, sf::Vector2f xBound, sf::Vector2f yBound) {

    std::vector<Ball> resolvedBalls;

    for (auto& ball : balls) {

        ball.position = { -1.f,-1.f };

        bool unresolved = true;

        while ((ball.position.x == -1 && ball.position.y == -1) || unresolved) {

            ball.position = generateIndividualRandomBallPosition(xBound, yBound);

            unresolved = false;

            for (int i = 0; i < resolvedBalls.size(); i++) {
                float distance = sqrtf(powf(ball.position.x - resolvedBalls[i].position.x, 2) + powf(ball.position.y - resolvedBalls[i].position.y, 2));


                if (distance <= (ball.radius + resolvedBalls[i].radius)) {
                    unresolved = true;
                    break;
                }
            }


        }

        resolvedBalls.push_back(ball);
    }
}