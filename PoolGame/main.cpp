#include <SFML/Graphics.hpp>
#include "Ball.h"
#include "Animation.h"
#include "AimShape.h"
#include "CuePowerBar.h"
#include "GameState.h"
#include "GameStateStack.h"
#include "WallBoundingBox.h"
#include "StereographicProjection.h"
#include "PathTracker.h"
#include "PathPredictor.h"

#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <cstdlib>
#include <time.h>
#include <math.h>

#define PI 3.14159265f


int main()
{

    // Load Ball String Names

    std::unordered_map<int,std::pair<std::string, sf::Image*>> ballFileNames;
    ballFileNames.insert({ 0, {"Pool Ball Skins/BallCue.jpg", nullptr } });
    ballFileNames.insert({1, {"Pool Ball Skins/Ball1.jpg",nullptr}});
    ballFileNames.insert({2, {"Pool Ball Skins/Ball2.jpg",nullptr}});
    ballFileNames.insert({3, {"Pool Ball Skins/Ball3.jpg",nullptr}});
    ballFileNames.insert({4, {"Pool Ball Skins/Ball4.jpg",nullptr}});
    ballFileNames.insert({5, {"Pool Ball Skins/Ball5.jpg",nullptr}});
    ballFileNames.insert({6, {"Pool Ball Skins/Ball6.jpg",nullptr}});
    ballFileNames.insert({7, {"Pool Ball Skins/Ball7.jpg",nullptr}});
    ballFileNames.insert({8, {"Pool Ball Skins/Ball8.jpg",nullptr}});
    ballFileNames.insert({9, {"Pool Ball Skins/Ball9.jpg",nullptr}});
    ballFileNames.insert({ 10, {"Pool Ball Skins/Ball10.jpg", nullptr} });
    ballFileNames.insert({11, {"Pool Ball Skins/Ball11.jpg",nullptr}});
    ballFileNames.insert({12, {"Pool Ball Skins/Ball12.jpg",nullptr}});
    ballFileNames.insert({13, {"Pool Ball Skins/Ball13.jpg",nullptr}});
    ballFileNames.insert({14, {"Pool Ball Skins/Ball14.jpg",nullptr}});
    ballFileNames.insert({ 15, {"Pool Ball Skins/Ball 15.jpg",nullptr} });

    for (auto itr = ballFileNames.begin(); itr != ballFileNames.end(); itr++)
    {
        sf::Image* image = new sf::Image;
        image->loadFromFile(itr->second.first);
        itr->second.second = image;
    }


    // Draw Screen based on presence of table
    bool drawTable = true;
    float theScale = 0.75f;
    float theLength;

    if (drawTable) {
        theLength = 1450.f; // was 1250.f
    }
    else {
        theLength = 750.f;
    }

    int screenWidth = (int)(theLength * theScale); // was (750.f * theScale)
    int screenHeight;

    if (drawTable) {
        screenHeight = (int)(screenWidth * (75.f / 131.f)); // was (1000.f * theScale)
    }
    else {
        screenHeight = (int)(1000.f * theScale);
    }

    int altScreenWidth = (int)(2000.f * theScale);
    int altScreenHeight = (int)(800.f * theScale);

    sf::Vector2i screenSize = { screenWidth, screenHeight };

    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "SFML works!");
    //sf::RenderWindow window(sf::VideoMode(altScreenWidth, altScreenHeight), "Alt SFML works!");
    
    // Try to resize window

    sf::Vector2i oldMouseCoordinates = { 0,0 };


    // --------------------------------------------------

    // Table Lines
    float width, height, innerWidth, innerHeight, ballSize, sidePocketSize, cornerPocketSize;

    // Table Dimensions
    height = screenWidth * 0.99f;
    width = (75.f / 131.f) * height;
    innerHeight = (112.f / 131.f) * height;
    innerWidth = (56.f / 75.f) * width;

    ballSize = (65.f / 2620.f) * height; // was 57.f/2620.f -> diameter
    sidePocketSize = (133.f / 2620.f) * height;
    cornerPocketSize = (117.f / 2620.f) * height;

    float outerWallSize;
    outerWallSize = (127.f / 2620.f) * height;

    float ballStartPosX = (2.f / 3.f) * height;

    float theThickness = (3.f / 1012.f) * height;

    // Key Table Dimensions (using ratios)
    float wallWidthDifference = (width - innerWidth) / 2; // f
    float gapBetweenInnerWidthHeight = (cornerPocketSize * sqrtf(2)) / 2.f; // n
    float f = wallWidthDifference;
    float n = gapBetweenInnerWidthHeight;
    float t = sidePocketSize;
    float h = outerWallSize;
    float s = cornerPocketSize;

    float aa, cc, dd, ee, ff, gg, hh, jj;
    aa = f + n;
    cc = f + innerHeight / 2 - t / 2;
    dd = f + innerHeight / 2 + t / 2;
    ee = f + innerHeight - n;
    ff = f + innerHeight;
    gg = f + innerWidth - n;
    hh = f + innerWidth;
    jj = f - h;

    float neg2Root2 = -sqrtf(2.f) / 2.f;
    float pos2Root2 = sqrtf(2.f) / 2.f;

    std::vector<std::vector<sf::Vector2f>> outerWallLines; // 1-4
    outerWallLines = { {{0.f,0.f},{0.f,width}}, // 1
                      {{0.f,0.f},{height,0.f}}, // 2
                      {{height,0.f},{height,width}}, // 3
                      {{0,width},{height,width}} }; // 4

    std::vector<std::vector<sf::Vector2f>> innerWallLines; // -> contact lines
    innerWallLines = { {{aa,f},{cc,f}}, // 5
                      {{dd,f},{ee,f}}, // 6
                      {{ff,aa},{ff,gg}}, // 7
                      {{ee,hh},{dd,hh}}, // 8
                      {{cc,hh},{aa,hh}}, // 9
                      {{f,gg},{f,aa}} }; // 10

    std::vector<std::vector<sf::Vector2f>> pocketWallLines; // -> contact lines
    pocketWallLines = { {{f,aa},{s * neg2Root2 + f,s * neg2Root2 + aa}}, // 11
                       {{aa,f},{s * neg2Root2 + aa,s * neg2Root2 + f}}, // 12
                       {{ee,f},{s * pos2Root2 + ee,s * neg2Root2 + f}}, // 13
                       {{ff,aa},{s * pos2Root2 + ff,s * neg2Root2 + aa}}, // 14
                       {{ff,gg},{s * pos2Root2 + ff,s * pos2Root2 + gg}}, // 15
                       {{ee,hh},{s * pos2Root2 + ee,s * pos2Root2 + hh}}, // 16
                       {{aa,hh},{s * neg2Root2 + aa,s * pos2Root2 + hh}}, // 17
                       {{f,gg},{s * neg2Root2 + f,s * pos2Root2 + gg}} }; // 18

    std::vector<std::vector<sf::Vector2f>> betweenWallLines;
    betweenWallLines = { {{aa,h},{cc,h}}, // 19
                        {{dd,h},{ee,h}}, // 20
                        {{dd,hh + jj},{ee,hh + jj}}, // 21
                        {{aa,hh + jj},{cc,hh + jj}}, // 22
                        {{h,aa},{h,gg}}, // 23
                        {{ff + jj,aa},{ff + jj,gg}} }; //24

    std::vector<std::vector<sf::Vector2f>> sidePocketWallLines; // -> contact lines
    sidePocketWallLines = { {{cc,f},{cc,h}},
                            {{dd,f},{dd,h}},
                            {{cc,hh},{cc,hh + jj}},
                            {{dd,hh},{dd,hh + jj}} };

    std::vector<std::vector<sf::Vector2f>> allTableLines; // These are all the table lines drawn on the screen
    std::vector<std::vector<sf::Vector2f>> contactTableLines; // These are the table lines that the ball may potentially contact with
    allTableLines.insert(allTableLines.end(), outerWallLines.begin(), outerWallLines.end());
    allTableLines.insert(allTableLines.end(), innerWallLines.begin(), innerWallLines.end());
    allTableLines.insert(allTableLines.end(), pocketWallLines.begin(), pocketWallLines.end());
    allTableLines.insert(allTableLines.end(), betweenWallLines.begin(), betweenWallLines.end());
    allTableLines.insert(allTableLines.end(), sidePocketWallLines.begin(), sidePocketWallLines.end());

    contactTableLines.insert(contactTableLines.end(), innerWallLines.begin(), innerWallLines.end());
    contactTableLines.insert(contactTableLines.end(), pocketWallLines.begin(), pocketWallLines.end());
    contactTableLines.insert(contactTableLines.end(), sidePocketWallLines.begin(), sidePocketWallLines.end());

    // Contact Table Lines
    std::vector<std::vector<float>> contactLineValues; // {slope, yIntercept, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y, realX, realY}
    for (auto theLine : contactTableLines) {
        float m, b;
        if (theLine[1].x - theLine[0].x == 0) {
            m = 0;
            b = 0;
        }
        else {
            m = (theLine[1].y - theLine[0].y) / (theLine[1].x - theLine[0].x);
            b = theLine[0].y - m * theLine[0].x;
        }

        if (abs(m) < 0.001f) {
            m = 0;
        }

        if (abs(b) < 0.001f) {
            b = 0;
        }

        sf::Vector2f topLeftPoint = { std::min(theLine[0].x,theLine[1].x), std::min(theLine[0].y,theLine[1].y) };
        sf::Vector2f bottomRightPoint = { std::max(theLine[0].x,theLine[1].x), std::max(theLine[0].y,theLine[1].y) };

        //std::cout << "m = " << m << "; b = " << b << std::endl;
        //std::cout << "Bounding Box: (" << topLeftPoint.x << "," << topLeftPoint.y << ") and (" << bottomRightPoint.x << "," << bottomRightPoint.y << ")" << std::endl;

        contactLineValues.push_back({ m,b,topLeftPoint.x, topLeftPoint.y, bottomRightPoint.x, bottomRightPoint.y, theLine[0].x });

    }

    // Table Circles - aka holes
    std::vector<sf::Vector2f> allTableCircles;
    allTableCircles = { {s * neg2Root2 + f + n / 2.f, s * neg2Root2 + f + n / 2.f},
                        {(cc + dd) / 2.f, h},
                        {s * pos2Root2 + ff - n / 2.f,s * neg2Root2 + f + n / 2.f},
                        {s * neg2Root2 + f + n / 2.f, s * pos2Root2 + hh - n / 2.f},
                        {(cc + dd) / 2.f,hh + jj},
                        {s * pos2Root2 + ff - n / 2.f, s * pos2Root2 + hh - n / 2.f} };

    bool rotateTable = false; // Rotate the table 90 degrees
    if (rotateTable) {
        for (auto& lines : allTableLines) {
            for (auto& otherLines : lines) {


                // Debug
                std::cout << "lines[i].y before = " << otherLines.y << std::endl;
                std::cout << "lines[i].x before = " << otherLines.x << std::endl;

                otherLines.y -= width;

                // Debug
                std::cout << "lines[i].y after = " << otherLines.y << std::endl;
                std::cout << "lines[i].x after = " << otherLines.x << std::endl;

                float oldX = otherLines.x;
                float oldY = otherLines.y;

                otherLines.x = oldX * 0.f + oldY * -1.f;
                otherLines.y = -oldX * -1.f + oldY * 0.f;

                // Debug
                std::cout << "lines[i].y modified = " << otherLines.y << std::endl;
                std::cout << "lines[i].x modified = " << otherLines.x << std::endl;
                std::cout << std::endl;

            }
        }
    }


    float theRate = 0.02f; // was 0.04 -> 0.02f seems to work perfectly (frame rate)


    std::vector<Ball> test3Balls;
    float yMove = width / 2.f;
    float xMove = ballStartPosX;

    Ball cueBall;
    cueBall.setUpBall({ (1.f / 5.f) * height, yMove }, 1.5f * theScale, ballFileNames[0].second, ballSize, 0);
    //cueBall.position = { (1.f / 5.f) * height, yMove };
    //cueBall.mass *= 1.f * theScale;
    //cueBall.radius = ballSize / 2.f;
    //cueBall.updateImpulse();
    //cueBall.ballImage = ballFileNames[0].second;
    ////if (!cueBall.ballImage->loadFromFile(ballFileNames[0].first)) {
    ////    return -1;
    ////}
    //cueBall.projectionImage.create(ballSize, ballSize, sf::Color::Black);
    //StereographicProjection step1(*cueBall.ballImage, cueBall.projectionImage, ballSize / 2.f, cueBall.returnAngles, cueBall.rotAngles);
    //cueBall.ballTexture = new sf::Texture;
    //if (!cueBall.ballTexture->loadFromImage(cueBall.projectionImage)) {
    //    return -1;
    //}
    //cueBall.drawBall();
    //cueBall.shape.setTexture(cueBall.ballTexture);
    //cueBall.shape.setTextureRect(sf::IntRect(0, 0, (int)ballSize, (int)ballSize));
    //cueBall.setBallNumber(0);
    test3Balls.push_back(cueBall);

    int ballImageCount = 1;

    for (int i = 0; i < 5; i++) {

        for (int j = 0; j <= i; j++) {
            Ball someBall;
            someBall.setUpBall({ xMove, yMove + j * (ballSize + 0.5f) }, 1.5f * theScale, ballFileNames[ballImageCount].second, ballSize, ballImageCount);

            //someBall.position = { xMove, yMove + j * (ballSize + 0.5f) };
            //someBall.mass *= 1.5f * theScale;
            //someBall.radius = (ballSize / 2.f);
            ////someBall.color = { 255,0,0 }; // red
            ////someBall.shape.setFillColor(sf::Color::Red);
            //someBall.updateImpulse();
            //someBall.ballImage = ballFileNames[ballImageCount].second;
            ////if (!someBall.ballImage->loadFromFile(ballFileNames[ballImageCount].first)) {
            ////    std::cout << "Image could not load; ballCount =  " << ballImageCount << std::endl;
            ////    return -1;
            ////}
            //someBall.projectionImage.create(ballSize, ballSize, sf::Color::Black);
            //StereographicProjection step2(*someBall.ballImage, someBall.projectionImage, ballSize / 2.f, someBall.returnAngles, someBall.rotAngles);
            //someBall.ballTexture = new sf::Texture;
            //if (!someBall.ballTexture->loadFromImage(someBall.projectionImage)) {
            //    return -1;
            //}
            //someBall.drawBall();
            //someBall.shape.setTexture(someBall.ballTexture);
            //someBall.shape.setTextureRect(sf::IntRect(0.f, 0.f, ballSize, ballSize));
            //someBall.setBallNumber(ballImageCount);
            test3Balls.push_back(someBall);

            ballImageCount++;

        }

        yMove -= (ballSize / 2.f);
        xMove += ballSize;
    }

    Animation animation;
    animation.balls = test3Balls;     // Load all 16 balls into an animation object

    bool haloMode = false; // draws halo around the ball -> halo radius is twice the radius of the ball

    int ballWithAimModeOnIndex = -1; // For aim mode
    std::vector<sf::RectangleShape> oldAimShapeLines;
    std::vector<sf::CircleShape> oldAimShapeCircles;

    float rightButtonDistanceMoved = 0.f; // track distance mouse moves from right click position

    sf::Vector2f cuePosition = { ff + sidePocketSize * 0.8f, f * 1.4f };
    sf::Vector2f cueSize = { 0.25f * h,innerWidth - 1.1f * h };

    sf::Vector2i firstRightClickPoint = { -1,-1 };
    sf::Vector2i rightClickPoint = { -1, -1 };
    sf::Vector2f resolvedSpeedsAimMode;


    sf::Vector2f tableXBounds = { f + ballSize / 2.f,ff - ballSize / 2.f };
    sf::Vector2f tableYBounds = { f + ballSize / 2.f,hh - ballSize / 2.f };

    GameState state;
    //state.generateRandomBallPositions(animation.balls, tableXBounds, tableYBounds);
    //state.loadGameStateFromEncryption("0.4310a.4c5d6;1.bd3fa.23f75;2.37a9b.7689e;d.79e16.6b653",
    //    animation.balls, ballFileNames, theScale, ballSize);
    GameStateStack gameStates(15);
    //state.loadGameStateFromEncryption("0.94495.5b18c;3.b78b5.63bb5;5.be47b.63bc3;9.5f1d8.63b82",
    //animation.balls, ballFileNames, theScale, ballSize);
    // 0.94495.5b18c;3.b78b5.63bb5;5.be47b.63bc3;9.5f1d8.63b82

    std::cout << "animation.balls.size() = " << animation.balls.size() << std::endl;

    bool doesMatter = false;

    WallBoundingBox wallBoxes;
    wallBoxes.boxes = { {{aa,0.f},{cc - aa,f}},
                        {{dd,0.f},{ee - dd,f}},
                        {{ff,aa},{f,gg - aa}},
                        {{dd,hh},{ee - dd,f}},
                        {{aa,hh},{cc - aa,f}},
                        {{0.f,aa},{f,gg - aa}} };


    std::vector<Ball> collectedBalls;

    bool rotateArrowUsingKeys = false;
    float startingAngle = 90;
    float angleIncrement = 0.5f;

    std::vector<float> holeTimeLeft(allTableCircles.size(), 0.f);
    float timeHightlighted = 3.f; // 3 seconds
    float aimLineAddLengthIncrement = 2.f;
    float currentAimLengthAddedLength = 2.f;
    float maxAimLineAddedLength = 100.f;

    bool gameStateLoadPause = true;
    bool turnOnWhiteHighlight = true; // turns on/off white highlight around balls
    unsigned int frameRateLimit = (int)(abs(1.f / theRate)); // should be equal to 50

    std::cout << "frameLimit = " << frameRateLimit << std::endl;

    float theFrictionFactor = 0.985f; // was 0.985

    unsigned int currentBallGroupSize = animation.balls.size();
    unsigned int previousBallGroupSize = 0;

    window.setFramerateLimit(50);

    float maxVelocityMagnitude = (1.3f * ballSize) / theRate;
    std::cout << "maxVelocityMagnitude = " << maxVelocityMagnitude << std::endl;

    PathTracker pathTracker(ballSize/2.f);
    int pathTrackerBallNumber = -1;

    bool pathTrackerMode = true;
    bool pathPredictorMode = true;

    float velocityCap = 2.8f; // was 0.1f

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Add Game States when velocities are 0
            if (event.type == sf::Event::KeyPressed) {
                // Debug
                //std::cout << "Key Pressed" << std::endl;

                if (event.key.code == sf::Keyboard::F) {

                    bool allVelocitiesAreZero = true;

                    for (auto i : animation.balls) {
                        if (i.velocity.x != 0.f || i.velocity.y != 0.f) {
                            allVelocitiesAreZero = false;
                            break;
                        }
                    }

                    if (allVelocitiesAreZero) {
                        GameState gState(animation.balls);
                        gState.printGameState();
                    }
                }
                else if (event.key.code == sf::Keyboard::H)
                {
                    if (turnOnWhiteHighlight)
                    {
                        turnOnWhiteHighlight = false;
                    }
                    else {
                        turnOnWhiteHighlight = true;
                    }
                }
                else if (event.key.code == sf::Keyboard::T)
                {
                    if (ballWithAimModeOnIndex == -1)
                    {
                        if (pathTrackerMode)
                        {
                            pathTrackerMode = false;
                        }
                        else {
                            pathTrackerMode = true;
                        }
                    }
                }
                else if (event.key.code == sf::Keyboard::B)
                {
                    if (pathPredictorMode)
                    {
                        pathPredictorMode = false;
                    }
                    else {
                        pathPredictorMode = true;
                    }
                }
                else if (event.key.code == sf::Keyboard::Escape)
                {
                    if (ballWithAimModeOnIndex != -1)
                    {
                        ballWithAimModeOnIndex = -1;
                    }
                }
                else if (event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left) {
                    if (ballWithAimModeOnIndex != -1 && rotateArrowUsingKeys) {
                        startingAngle -= angleIncrement;
                    }
                }
                else if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right) {
                    if (ballWithAimModeOnIndex != -1 && rotateArrowUsingKeys) {
                        startingAngle += angleIncrement;
                    }
                }
                else if (event.key.code == sf::Keyboard::K) {
                    if (rotateArrowUsingKeys == false) {
                        rotateArrowUsingKeys = true;
                    }
                    else {
                        rotateArrowUsingKeys = false;
                    }
                }
                else if (event.key.code == sf::Keyboard::Equal) {
                    currentAimLengthAddedLength += aimLineAddLengthIncrement;

                    currentAimLengthAddedLength = std::min(maxAimLineAddedLength, currentAimLengthAddedLength);
                    currentAimLengthAddedLength = std::max(2.f, currentAimLengthAddedLength);

                    std::cout << "= sign pressed" << std::endl;

                    std::cout << "currentAimLengthAddedLength = " << currentAimLengthAddedLength << std::endl;
                }
                else if (event.key.code == sf::Keyboard::Dash) {
                    currentAimLengthAddedLength -= aimLineAddLengthIncrement;

                    currentAimLengthAddedLength = std::min(maxAimLineAddedLength, currentAimLengthAddedLength);
                    currentAimLengthAddedLength = std::max(2.f, currentAimLengthAddedLength);

                    std::cout << "- sign pressed" << std::endl;

                    std::cout << "currentAimLengthAddedLength = " << currentAimLengthAddedLength << std::endl;
                }
                else if (event.key.code == sf::Keyboard::Z || event.key.code == sf::Keyboard::R || event.key.code == sf::Keyboard::P) {

                    bool allVelsAreZero = true;

                    for (auto ball : animation.balls) {
                        if (ball.velocity.x != 0.f || ball.velocity.y != 0.f) {
                            allVelsAreZero = false;
                            break;
                        }
                    }

                    if (event.key.code == sf::Keyboard::Z) {

                        if (allVelsAreZero) {
                            gameStates.undo();

                            std::cout << "Undo called" << std::endl;

                            state.loadGameStateFromGameState(gameStates.loadGameState(), animation.balls, ballFileNames, theScale, ballSize);

                            //if (gameStates.currentGameState > 0) {


                            //    // Debug
                            //    std::cout << "Undo called" << std::endl;

                            //    state.loadGameStateFromGameState(gameStates.loadGameState(), animation.balls, ballFileNames, theScale, ballSize);
                            //}
                        }
                    }
                    else if (event.key.code == sf::Keyboard::R) {
                        if (allVelsAreZero) {
                            gameStates.redo();

                            std::cout << "Redo called" << std::endl;

                            state.loadGameStateFromGameState(gameStates.loadGameState(), animation.balls, ballFileNames, theScale, ballSize);

                            //if (gameStates.currentGameState < gameStates.maxLimit - 1) {
                            //    state.loadGameStateFromGameState(gameStates.loadGameState(), animation.balls, ballFileNames, theScale, ballSize);
                            //}
                        }
                    }
                    else if (event.key.code == sf::Keyboard::P)
                    {
                        if (allVelsAreZero) {
                            if (!gameStates.allGameStates.empty())
                            {
                                std::cout << "Game States: " << std::endl;
                                gameStates.printMostRecentGameState();
                            }
                            else {
                                std::cout << "gameStates vector is empty!" << std::endl;
                            }
                        }
                    }

                }
            }
        }

        float realT = 0.f;
        float timeRemaining = theRate;

        float frictionFactor = 1.2f; // was 0.99f


        //-----------------------------------------------

        window.clear();

        // Resolve Movement
        if (true) {

            // Adjusting Velocities / Angular Velocities / Impulses
            for (auto& theBall : animation.balls) {

                // Y-Component

                //animation.updateFNetAndTorqueYAxis();


                //theBall.velocity.y = theBall.velocity.y + theBall.frictionCoefficient.y * animation.gravity * theRate;
                theBall.velocity.y *= theFrictionFactor;
                theBall.angularVelocity.x = theBall.angularVelocity.x + (5.f * animation.frictionCoefficient * animation.gravity * theRate * 180.f)
                    / (2.f * theBall.radius * 3.14f);
                theBall.angularMomentum.x = (theBall.inertia * 3.14f * theBall.angularVelocity.x) / (180.f);
                theBall.angularPosition.x = theBall.angularPosition.x + theBall.angularVelocity.x * theRate;
                theBall.impulse.y = (theBall.mass * theBall.velocity.y);



                // X-Component

                //animation.updateFNetAndTorqueXAxis();

                //theBall.velocity.x = theBall.velocity.x + theBall.frictionCoefficient.x * animation.gravity * theRate;
                theBall.velocity.x *= theFrictionFactor;
                theBall.angularVelocity.y = theBall.angularVelocity.y + (5.f * animation.frictionCoefficient * animation.gravity * theRate * 180.f)
                    / (2.f * theBall.radius * 3.14f);
                theBall.angularMomentum.y = (theBall.inertia * 3.14f * theBall.angularVelocity.y) / (180.f);
                theBall.angularPosition.y = theBall.angularPosition.y + theBall.angularVelocity.y * theRate;
                theBall.impulse.x = (theBall.mass * theBall.velocity.x);
                theBall.updateImpulse();

                theBall.timeLeft = theRate;
            }

            // Ball-Ball Collisions
            for (int m = 0; m < animation.balls.size() - 1; m++) {

                Ball& ballS1 = animation.balls[m];

                sf::Vector2f ballOriginalPosition = ballS1.position;

                for (int n = m + 1; n < animation.balls.size(); n++) {

                    Ball& ballS2 = animation.balls[n];

                    timeRemaining = theRate;

                    if (abs(ballS1.oldVelocity.x) <= 0.f && abs(ballS1.oldVelocity.y) <= 0.f
                        && abs(ballS2.oldVelocity.x) <= 0.f && abs(ballS2.oldVelocity.y) <= 0.f)
                    {
                        continue;
                    }

                    if (ballS1.ballCollision(ballS2)) {


                        //std::cout << "m = " << m << "; n = " << n << std::endl;
                        //std::cout << "timeRemaining = " << timeRemaining << std::endl;
                        //std::cout << "Ball 1 position = (" << ballS1.position.x << "," << ballS1.position.y << ")" << std::endl;
                        //std::cout << "Ball 2 position = (" << ballS2.position.x << "," << ballS2.position.y << ")" << std::endl;

                        // Quadratic Formula

                        float x10 = ballS1.position.x - ballS1.oldVelocity.x * theRate;
                        float y10 = ballS1.position.y - ballS1.oldVelocity.y * theRate;
                        float x20 = ballS2.position.x - ballS2.oldVelocity.x * theRate;
                        float y20 = ballS2.position.y - ballS2.oldVelocity.y * theRate;

                        //float fDistance = sqrtf(powf(ballS1.position.x - ballS2.position.x,2) + powf(ballS1.position.y - ballS2.position.y,2) );

                        //// Calculate displacement required
                        //float fOverlap = 0.5f * (fDistance - ballS1.radius - ballS2.radius);

                        //// Displace Current Ball away from collision
                        //ballS1.position.x -= fOverlap * (ballS1.position.x - ballS2.position.x) / fDistance;
                        //ballS1.position.y -= fOverlap * (ballS1.position.y - ballS2.position.y) / fDistance;

                        //// Displace Target Ball away from collision - Note, this should affect the timing of the target ball
                        //// and it does, but this is absorbed by the target ball calculating its own time delta later on
                        //ballS2.position.x += fOverlap * (ballS1.position.x - ballS2.position.x) / fDistance;
                        //ballS2.position.y += fOverlap * (ballS1.position.y - ballS2.position.y) / fDistance;

                        float a = powf(ballS2.oldVelocity.x - ballS1.oldVelocity.x, 2) + powf(ballS2.oldVelocity.y - ballS1.oldVelocity.y, 2);
                        float b = 2.f * ((x20 - x10) * (ballS2.oldVelocity.x - ballS1.oldVelocity.x) + (y20 - y10) * (ballS2.oldVelocity.y - ballS1.oldVelocity.y));
                        float c = powf(x20 - x10, 2) + powf(y20 - y10, 2) - powf(ballS1.radius + ballS2.radius, 2);

                        float posT = (-b + sqrtf(powf(b, 2) - 4.f * a * c)) / (2.f * a);
                        float negT = (-b - sqrtf(powf(b, 2) - 4.f * a * c)) / (2.f * a);



                        if (negT >= 0.f) {
                            realT = negT; // usually > 0

                            if (ballS1.timeLeft != theRate) {
                                ballS1.timeLeft = std::max(theRate - realT, ballS1.timeLeft);
                            }
                            else {
                                ballS1.timeLeft = theRate - realT;
                            }


                            if (ballS2.timeLeft != theRate) {
                                ballS2.timeLeft = std::max(theRate - realT, ballS2.timeLeft);
                            }
                            else {
                                ballS2.timeLeft = theRate - realT;
                            }

                        }
                        else {
                            realT = posT;

                            if (ballS1.timeLeft != theRate) {
                                ballS1.timeLeft = std::max(theRate - realT, ballS1.timeLeft);
                            }
                            else {
                                ballS1.timeLeft = theRate - realT;
                            }


                            if (ballS2.timeLeft != theRate) {
                                ballS2.timeLeft = std::max(theRate - realT, ballS2.timeLeft);
                            }
                            else {
                                ballS2.timeLeft = theRate - realT;
                            }
                        }

                        // Debug
                        //std::cout << "NEGT = " << negT << std::endl;
                        //std::cout << "POST = " << posT << std::endl;

                        // x = x0 + vt
                        ballS1.position.x = x10 + (realT * ballS1.oldVelocity.x);
                        ballS1.position.y = y10 + (realT * ballS1.oldVelocity.y);

                        ballS2.position.x = x20 + (realT * ballS2.oldVelocity.x);
                        ballS2.position.y = y20 + (realT * ballS2.oldVelocity.y);

                        sf::Vector2f normalVector(ballS2.position.x - ballS1.position.x, ballS2.position.y - ballS1.position.y);
                        float normalMagnitude = sqrtf(powf(normalVector.x, 2) + powf(normalVector.y, 2));

                        normalVector.x /= normalMagnitude;
                        normalVector.y /= normalMagnitude;

                        sf::Vector2f tangentVector(-normalVector.y, normalVector.x); // unit tangent vector

                        float normal1Scalar = normalVector.x * ballS1.oldVelocity.x + normalVector.y * ballS1.oldVelocity.y;
                        float tangent1Scalar = tangentVector.x * ballS1.oldVelocity.x + tangentVector.y * ballS1.oldVelocity.y;
                        float normal2Scalar = normalVector.x * ballS2.oldVelocity.x + normalVector.y * ballS2.oldVelocity.y;
                        float tangent2Scalar = tangentVector.x * ballS2.oldVelocity.x + tangentVector.y * ballS2.oldVelocity.y;

                        float normalSpeedAfter1 = ((ballS1.mass - ballS2.mass) * normal1Scalar + 2.f * ballS2.mass * normal2Scalar) / (ballS1.mass + ballS2.mass);
                        float normalSpeedAfter2 = ((2.f * ballS1.mass * normal1Scalar) + (ballS2.mass - ballS1.mass) * normal2Scalar) / (ballS1.mass + ballS2.mass);

                        sf::Vector2f normalVector1After(normalSpeedAfter1 * normalVector.x, normalSpeedAfter1 * normalVector.y);
                        sf::Vector2f normalVector2After(normalSpeedAfter2 * normalVector.x, normalSpeedAfter2 * normalVector.y);

                        // Vt = v't
                        sf::Vector2f tangentVector1After(tangent1Scalar * tangentVector.x, tangent1Scalar * tangentVector.y);
                        sf::Vector2f tangentVector2After(tangent2Scalar * tangentVector.x, tangent2Scalar * tangentVector.y);

                        // Debug
                        //std::cout << "Before: " << std::endl;
                        //std::cout << "ball1.velocity.x = " << ballS1.velocity.x << std::endl;
                        //std::cout << "ball1.velocity.y = " << ballS1.velocity.y << std::endl;
                        //std::cout << "ball2.velocity.x = " << ballS2.velocity.x << std::endl;
                        //std::cout << "ball2.velocity.y = " << ballS2.velocity.y << std::endl;

                        ballS1.velocity.x = (normalVector1After.x + tangentVector1After.x);
                        ballS1.velocity.y = (normalVector1After.y + tangentVector1After.y);
                        ballS2.velocity.x = (normalVector2After.x + tangentVector2After.x);
                        ballS2.velocity.y = (normalVector2After.y + tangentVector2After.y);

                        // Debug
                        std::cout << "ballS1.velocity = (" << ballS1.velocity.x << "," << ballS1.velocity.y << ")" << std::endl;
                        std::cout << "ballS2.velocity = (" << ballS2.velocity.x << "," << ballS2.velocity.y << ")" << std::endl;
                        std::cout << std::endl;


                    }

                }
            }

            int ballCount = -1; // Debug

            // Ball Contact With Table Lines
            for (auto& aBall : animation.balls) {

                int tableCount = -1;
                ballCount++;

                // If the ball is in contact with a wall and stationary, then continue (don't move the stationary ball)
                if (aBall.oldVelocity.x == 0.f && aBall.oldVelocity.y == 0.f) {
                    continue;
                }

                // Determine which table line is intersecting with the ball
                for (auto line : contactTableLines) {

                    tableCount++;

                    sf::Vector2f pS = { aBall.position.x - line[0].x, aBall.position.y - line[0].y };
                    sf::Vector2f eS = { line[1].x - line[0].x, line[1].y - line[0].y };

                    float lengthES = (powf(eS.x, 2) + powf(eS.y, 2));

                    float tPoint;

                    tPoint = std::max(0.f, std::min(lengthES, (pS.x * eS.x + pS.y * eS.y))) / lengthES;

                    sf::Vector2f closestPoint = { line[0].x + (tPoint * eS.x),
                                                    line[0].y + (tPoint * eS.y) };

                    float theDistance = sqrtf(powf(aBall.position.x - closestPoint.x, 2) + powf(aBall.position.y - closestPoint.y, 2));

                    if (theDistance <= aBall.radius) {

                        float x10 = aBall.position.x - aBall.oldVelocity.x * theRate;
                        float y10 = aBall.position.y - aBall.oldVelocity.y * theRate;

                        sf::Vector2f ballToClosestPoint, ballOldVelocityVector, normalWallLine;

                        ballToClosestPoint = { aBall.position.x - x10, aBall.position.y - y10 };
                        ballOldVelocityVector = { aBall.oldVelocity.x, aBall.oldVelocity.y };
                        normalWallLine = { -eS.y, eS.x };

                        float ballOldVelocityMagnitude = sqrtf(powf(ballOldVelocityVector.x, 2) + powf(ballOldVelocityVector.y, 2));
                        float ballToClosestPointMagnitude = sqrtf(powf(ballToClosestPoint.x, 2) + powf(ballToClosestPoint.y, 2));

                        ballToClosestPoint.x /= ballToClosestPointMagnitude;
                        ballToClosestPoint.y /= ballToClosestPointMagnitude;

                        ballOldVelocityVector.x /= ballOldVelocityMagnitude;
                        ballOldVelocityVector.y /= ballOldVelocityMagnitude;

                        normalWallLine.x *= -1.f;
                        normalWallLine.y *= -1.f;
                        normalWallLine.x /= sqrtf(lengthES);
                        normalWallLine.y /= sqrtf(lengthES);

                        float cosThetaD1 = (ballOldVelocityVector.x * normalWallLine.x + ballOldVelocityVector.y * normalWallLine.y);

                        sf::Vector2f currentBallClosestPointVector, originalBallClosestPointVector;
                        currentBallClosestPointVector = { closestPoint.x - aBall.position.x, closestPoint.y - aBall.position.y };
                        originalBallClosestPointVector = { closestPoint.x - x10, closestPoint.y - y10 };

                        float currentBallCPMagnitude = sqrtf(powf(currentBallClosestPointVector.x, 2) + powf(currentBallClosestPointVector.y, 2));
                        float originalBallCPMagnitude = sqrtf(powf(originalBallClosestPointVector.x, 2) + powf(originalBallClosestPointVector.y, 2));

                        currentBallClosestPointVector.x /= currentBallCPMagnitude;
                        currentBallClosestPointVector.y /= currentBallCPMagnitude;

                        originalBallClosestPointVector.x /= originalBallCPMagnitude;
                        originalBallClosestPointVector.y /= originalBallCPMagnitude;

                        float oBallcBallDotProduct = currentBallClosestPointVector.x * originalBallClosestPointVector.x + currentBallClosestPointVector.y * originalBallClosestPointVector.y;
                        float useThisDistance;

                        if (oBallcBallDotProduct > 0.f) {
                            useThisDistance = -theDistance;
                        }
                        else {
                            useThisDistance = theDistance;
                        }

                        float d1 = (aBall.radius + useThisDistance) / abs(cosThetaD1);
                        float d2 = ballToClosestPointMagnitude - d1;


                        if (true) {
                            aBall.position.x = x10 + ballOldVelocityVector.x * d2;
                            aBall.position.y = y10 + ballOldVelocityVector.y * d2;
                        }
                        else {

                            sf::CircleShape otherCircle(aBall.radius);
                            otherCircle.setOrigin({ aBall.radius,aBall.radius });
                            otherCircle.setPosition({ x10,y10 });
                            otherCircle.setFillColor(sf::Color::Cyan);
                            window.draw(otherCircle);


                            sf::CircleShape someCircle(aBall.radius);
                            someCircle.setOrigin({ aBall.radius,aBall.radius });
                            someCircle.setPosition({ x10 + ballOldVelocityVector.x * d2,y10 + ballOldVelocityVector.y * d2 });
                            someCircle.setFillColor(sf::Color::Green);
                            window.draw(someCircle);

                            sf::Vertex vertices[] = { sf::Vertex(someCircle.getPosition()),
                                sf::Vertex({someCircle.getPosition().x + ballOldVelocityVector.x * ballOldVelocityMagnitude,
                                    someCircle.getPosition().y + ballOldVelocityVector.y * ballOldVelocityMagnitude}) };

                            window.draw(vertices, 2, sf::Lines);
                            //break;
                        }

                        // Debug
                        std::cout << "eS = (" << eS.x << "," << eS.y << ")" << std::endl;
                        std::cout << "ballOldVelocityVector = (" << ballOldVelocityVector.x << "," << ballOldVelocityVector.y << ")" << std::endl;
                        std::cout << "ballToClosestPoint = (" << ballToClosestPoint.x << "," << ballToClosestPoint.y << ")" << std::endl;
                        std::cout << "normalWallLine = (" << normalWallLine.x << "," << normalWallLine.y << ")" << std::endl;
                        std::cout << "cosThetaD1 = " << cosThetaD1 << std::endl;


                        if (abs(aBall.oldVelocity.x) <= 0.f && abs(aBall.oldVelocity.y) <= 0.f) {

                            // Debug
                            std::cout << "SPECIAL CASE" << std::endl;
                            std::cout << std::endl;

                            continue;
                        }


                        Ball fakeBall;
                        fakeBall.position = closestPoint;
                        fakeBall.velocity.x = 0.f;
                        fakeBall.velocity.y = 0.f;
                        fakeBall.mass = aBall.mass;
                        fakeBall.radius = 0.f;

                        float x20 = fakeBall.position.x;
                        float y20 = fakeBall.position.y;

                        // Then, do the ball collision procedure
                        float a = powf(fakeBall.velocity.x - aBall.oldVelocity.x, 2) + powf(fakeBall.velocity.y - aBall.oldVelocity.y, 2);
                        float b = 2.f * ((x20 - x10) * (fakeBall.velocity.x - aBall.oldVelocity.x) + (y20 - y10) * (fakeBall.velocity.y - aBall.oldVelocity.y));
                        float c = powf(x20 - x10, 2) + powf(y20 - y10, 2) - powf(aBall.radius + fakeBall.radius, 2);

                        float posT = (-b + sqrtf(powf(b, 2) - 4.f * a * c)) / (2.f * a);
                        float negT = (-b - sqrtf(powf(b, 2) - 4.f * a * c)) / (2.f * a);

                        if (negT >= 0.f) {
                            realT = negT; // usually > 0
                            aBall.timeLeft -= realT;
                        }
                        else {
                            realT = posT;
                            aBall.timeLeft -= realT;
                        }

                        // Debug
                        //std::cout << "NEGT = " << negT << std::endl;
                        //std::cout << "POST = " << posT << std::endl;

                        if (negT < 0.f) {
                            std::cout << "negT less than 0; Table Line = " << tableCount << std::endl;
                            //std::cout << "ball Velocity = (" << aBall.velocity.x << "," << aBall.velocity.y << ")" << std::endl;
                        }

                        if (true) {
                            std::cout << "ball Velocity before = (" << aBall.oldVelocity.x << "," << aBall.oldVelocity.y << ")" << std::endl;
                            std::cout << "Time Left = " << aBall.timeLeft << std::endl;
                            std::cout << std::endl;

                        }

                        sf::Vector2f reflectedVelocity;
                        sf::Vector2f normalizedNormalVector = { -(line[1].y - line[0].y),line[1].x - line[0].x };

                        if (line[1].y - line[0].y == 0.f) {
                            normalizedNormalVector.x = 0.f;
                        }

                        float normalVectorMagnitude = sqrtf(powf(normalizedNormalVector.x, 2) + powf(normalizedNormalVector.y, 2));
                        normalizedNormalVector.x /= normalVectorMagnitude;
                        normalizedNormalVector.y /= normalVectorMagnitude;
                        float normalVelocityVectorDotProduct = normalizedNormalVector.x * aBall.oldVelocity.x + normalizedNormalVector.y * aBall.oldVelocity.y;
                        reflectedVelocity.x = aBall.oldVelocity.x - 2.f * normalVelocityVectorDotProduct * normalizedNormalVector.x;
                        reflectedVelocity.y = aBall.oldVelocity.y - 2.f * normalVelocityVectorDotProduct * normalizedNormalVector.y;

                        //float m = -normalizedNormalVector.x / normalizedNormalVector.y;

                        aBall.velocity = reflectedVelocity;



                        break;

                    }

                }

                int wallBoxCount = 0;

                // Determine if the ball is in one of the "wall boxes" -> this is done in cases where the ball has gone outside the main table
                for (auto box : wallBoxes.boxes) {
                    if (aBall.position.x >= box[0].x && aBall.position.x <= box[0].x + box[1].x
                        && aBall.position.y >= box[0].y && aBall.position.y <= box[0].y + box[1].y)
                    {

                        // DEbug
                        std::cout << "WALL BOX Called" << std::endl;

                        float x10, y10;
                        x10 = aBall.position.x - aBall.oldVelocity.x * theRate;
                        y10 = aBall.position.y - aBall.oldVelocity.y * theRate;

                        sf::Vector2f eS = { contactTableLines[wallBoxCount][1].x - contactTableLines[wallBoxCount][0].x,
                                            contactTableLines[wallBoxCount][1].y - contactTableLines[wallBoxCount][0].y };
                        sf::Vector2f pS = { aBall.position.x - contactTableLines[wallBoxCount][0].x,
                                            aBall.position.y - contactTableLines[wallBoxCount][0].y };

                        float lengthES = (powf(eS.x, 2) + powf(eS.y, 2));

                        float tPoint;

                        tPoint = std::max(0.f, std::min(lengthES, (pS.x * eS.x + pS.y * eS.y))) / lengthES;

                        sf::Vector2f closestPoint = { contactTableLines[wallBoxCount][0].x + (tPoint * eS.x),
                            contactTableLines[wallBoxCount][0].y + (tPoint * eS.y) };

                        float theDistance = sqrtf(powf(aBall.position.x - closestPoint.x, 2) + powf(aBall.position.y - closestPoint.y, 2));

                        sf::Vector2f ballToClosestPoint, ballOldVelocityVector, normalWallLine;

                        ballToClosestPoint = { aBall.position.x - x10, aBall.position.y - y10 };
                        ballOldVelocityVector = { aBall.oldVelocity.x, aBall.oldVelocity.y };
                        normalWallLine = { -eS.y, eS.x };

                        float ballOldVelocityMagnitude = sqrtf(powf(ballOldVelocityVector.x, 2) + powf(ballOldVelocityVector.y, 2));
                        float ballToClosestPointMagnitude = sqrtf(powf(ballToClosestPoint.x, 2) + powf(ballToClosestPoint.y, 2));

                        ballToClosestPoint.x /= ballToClosestPointMagnitude;
                        ballToClosestPoint.y /= ballToClosestPointMagnitude;

                        ballOldVelocityVector.x /= ballOldVelocityMagnitude;
                        ballOldVelocityVector.y /= ballOldVelocityMagnitude;

                        normalWallLine.x *= -1.f;
                        normalWallLine.y *= -1.f;
                        normalWallLine.x /= sqrtf(lengthES);
                        normalWallLine.y /= sqrtf(lengthES);

                        float cosThetaD1 = (ballOldVelocityVector.x * normalWallLine.x + ballOldVelocityVector.y * normalWallLine.y);

                        sf::Vector2f currentBallClosestPointVector, originalBallClosestPointVector;
                        currentBallClosestPointVector = { closestPoint.x - aBall.position.x, closestPoint.y - aBall.position.y };
                        originalBallClosestPointVector = { closestPoint.x - x10, closestPoint.y - y10 };

                        float currentBallCPMagnitude = sqrtf(powf(currentBallClosestPointVector.x, 2) + powf(currentBallClosestPointVector.y, 2));
                        float originalBallCPMagnitude = sqrtf(powf(originalBallClosestPointVector.x, 2) + powf(originalBallClosestPointVector.y, 2));

                        currentBallClosestPointVector.x /= currentBallCPMagnitude;
                        currentBallClosestPointVector.y /= currentBallCPMagnitude;

                        originalBallClosestPointVector.x /= originalBallCPMagnitude;
                        originalBallClosestPointVector.y /= originalBallCPMagnitude;

                        float oBallcBallDotProduct = currentBallClosestPointVector.x * originalBallClosestPointVector.x + currentBallClosestPointVector.y * originalBallClosestPointVector.y;
                        float useThisDistance = theDistance;

                        float d1 = (aBall.radius + useThisDistance) / abs(cosThetaD1);
                        float d2 = ballToClosestPointMagnitude - d1;


                        if (true) {
                            aBall.position.x = x10 + ballOldVelocityVector.x * d2;
                            aBall.position.y = y10 + ballOldVelocityVector.y * d2;

                        }
                        else {

                            sf::CircleShape otherCircle(aBall.radius);
                            otherCircle.setOrigin({ aBall.radius,aBall.radius });
                            otherCircle.setPosition({ x10,y10 });
                            otherCircle.setFillColor(sf::Color::Cyan);
                            window.draw(otherCircle);


                            sf::CircleShape someCircle(aBall.radius);
                            someCircle.setOrigin({ aBall.radius,aBall.radius });
                            someCircle.setPosition({ x10 + ballOldVelocityVector.x * d2,y10 + ballOldVelocityVector.y * d2 });
                            someCircle.setFillColor(sf::Color::Green);
                            window.draw(someCircle);

                            sf::Vertex vertices[] = { sf::Vertex(someCircle.getPosition()),
                                sf::Vertex({someCircle.getPosition().x + ballOldVelocityVector.x * ballOldVelocityMagnitude,
                                    someCircle.getPosition().y + ballOldVelocityVector.y * ballOldVelocityMagnitude}) };

                            window.draw(vertices, 2, sf::Lines);
                            //break;


                        }

                        if (abs(aBall.oldVelocity.x) <= 0.f && abs(aBall.oldVelocity.y) <= 0.f) {

                            // Debug
                            std::cout << "SPECIAL CASE" << std::endl;
                            std::cout << std::endl;

                            continue;
                        }


                        Ball fakeBall;
                        fakeBall.position = closestPoint;
                        fakeBall.velocity.x = 0.f;
                        fakeBall.velocity.y = 0.f;
                        fakeBall.mass = aBall.mass;
                        fakeBall.radius = 0.f;

                        float x20 = fakeBall.position.x;
                        float y20 = fakeBall.position.y;

                        // Then, do the ball collision procedure
                        float a = powf(fakeBall.velocity.x - aBall.oldVelocity.x, 2) + powf(fakeBall.velocity.y - aBall.oldVelocity.y, 2);
                        float b = 2.f * ((x20 - x10) * (fakeBall.velocity.x - aBall.oldVelocity.x) + (y20 - y10) * (fakeBall.velocity.y - aBall.oldVelocity.y));
                        float c = powf(x20 - x10, 2) + powf(y20 - y10, 2) - powf(aBall.radius + fakeBall.radius, 2);

                        float posT = (-b + sqrtf(powf(b, 2) - 4.f * a * c)) / (2.f * a);
                        float negT = (-b - sqrtf(powf(b, 2) - 4.f * a * c)) / (2.f * a);

                        if (negT >= 0.f) {
                            realT = negT; // usually > 0
                            aBall.timeLeft -= realT;
                        }
                        else {
                            realT = posT;
                            aBall.timeLeft -= realT;
                        }

                        // Debug
                        //std::cout << "NEGT = " << negT << std::endl;
                        //std::cout << "POST = " << posT << std::endl;

                        if (negT < 0.f) {
                            std::cout << "negT less than 0; Table Line = " << tableCount << std::endl;
                            //std::cout << "ball Velocity = (" << aBall.velocity.x << "," << aBall.velocity.y << ")" << std::endl;
                        }

                        if (true) {
                            std::cout << "ball Velocity before = (" << aBall.oldVelocity.x << "," << aBall.oldVelocity.y << ")" << std::endl;
                            std::cout << "Time Left = " << aBall.timeLeft << std::endl;
                            std::cout << std::endl;

                        }


                        sf::Vector2f reflectedVelocity;
                        sf::Vector2f normalizedNormalVector = { -(contactTableLines[wallBoxCount][1].y - contactTableLines[wallBoxCount][0].x),
                                                        contactTableLines[wallBoxCount][1].x - contactTableLines[wallBoxCount][0].x };

                        if (contactTableLines[wallBoxCount][1].y - contactTableLines[wallBoxCount][0].y == 0.f) {
                            normalizedNormalVector.x = 0.f;
                        }

                        float normalVectorMagnitude = sqrtf(powf(normalizedNormalVector.x, 2) + powf(normalizedNormalVector.y, 2));
                        normalizedNormalVector.x /= normalVectorMagnitude;
                        normalizedNormalVector.y /= normalVectorMagnitude;
                        float normalVelocityVectorDotProduct = normalizedNormalVector.x * aBall.oldVelocity.x + normalizedNormalVector.y * aBall.oldVelocity.y;
                        reflectedVelocity.x = aBall.oldVelocity.x - 2.f * normalVelocityVectorDotProduct * normalizedNormalVector.x;
                        reflectedVelocity.y = aBall.oldVelocity.y - 2.f * normalVelocityVectorDotProduct * normalizedNormalVector.y;


                        aBall.velocity = reflectedVelocity;

                        //}

                        break;


                    }

                    wallBoxCount++;
                }

            }

            // Movement / Displacement / Velocity Adjustment
            for (auto& ball : animation.balls) {

                if (sqrtf(powf(ball.velocity.x, 2) + powf(ball.velocity.y, 2)) < velocityCap) {
                    ball.velocity.x = 0.f;
                    ball.velocity.y = 0.f;
                }

                if (abs(ball.angularVelocity.x) < 0.1f) {
                    ball.angularVelocity.x = 0.f;
                }

                if (abs(ball.angularVelocity.y) < 0.1f) {
                    ball.angularVelocity.y = 0.f;
                }

                //ball.updateImpulse();

                animation.updateFNetAndTorqueXAxis();
                animation.updateFNetAndTorqueYAxis();

                ball.position.x = ball.position.x + ball.velocity.x * (ball.timeLeft); // FIX -> cases when v is greater than wr
                ball.position.y = ball.position.y + ball.velocity.y * (ball.timeLeft);

                ball.oldVelocity = ball.velocity;
            }

            int circCount = 0;

            // Determine if a ball has fallen into any of the six holes
            for (auto i : allTableCircles) {

                float holeRad;

                if (circCount == 1 || circCount == 4) {
                    holeRad = sidePocketSize / 2.f;
                }
                else {
                    holeRad = cornerPocketSize / 2.f;
                }

                ballCount = 0;
                for (auto& ball : animation.balls) {
                    float holeBallDistance = sqrtf(powf(i.x - ball.position.x, 2) + powf(i.y - ball.position.y, 2));

                    if (holeBallDistance <= (holeRad)) {


                        delete ball.ballTexture;
                        ball.ballImage = nullptr;
                        delete ball.ballImage;

                        collectedBalls.push_back(ball);
                        animation.balls.erase(animation.balls.begin() + ballCount);
                        ballCount--;
                        holeTimeLeft[circCount] += timeHightlighted;
                    }

                    ballCount++;
                }

                circCount++;
            }

            currentBallGroupSize = animation.balls.size(); // keeps track of remaining balls after accounting for all the balls 
                                                            // that have dropped in hole / removed from table
        }

        // Break if all the balls have cleared the table
        if (animation.balls.empty())
        {
            std::cout << "No more balls left" << std::endl;
            break;
        }

        // Draw Table Lines and Pool Balls
        if (true) {

            // Get the start and endpoints of each table line
            for (auto line : allTableLines) {
                sf::Vertex someLine[] = {
                    sf::Vertex(line[0]),
                    sf::Vertex(line[1]),
                };

                float lineLength = sqrtf(powf(line[1].x - line[0].x, 2) + powf(line[1].y - line[0].y, 2));
                float dotProduct = ((line[1].x - line[0].x) * 1.f + (line[1].y - line[0].y) * 0.f) / lineLength;
                float crossProduct = ((line[1].y - line[0].y) * 1.f + (line[1].x - line[0].x) * 0.f) / lineLength;

                float tanTheta = atan2f(crossProduct, dotProduct);
                tanTheta *= (180.f / PI);

                sf::RectangleShape rectLine;
                rectLine.setSize({ lineLength, theThickness });
                rectLine.setOrigin({ 0.f, theThickness / 2.f });
                rectLine.setPosition(line[0]);
                rectLine.setFillColor(sf::Color::White);
                rectLine.rotate(tanTheta);

                //window.draw(someLine, 2, sf::Lines);
                window.draw(rectLine);
            }

            int count = 0;

            // Draw the table circles (aka the holes)
            for (auto circles : allTableCircles) {
                sf::CircleShape circleShape;

                if (count == 1 || count == 4) {
                    circleShape.setRadius(sidePocketSize / 2.f);
                    circleShape.setOrigin({ sidePocketSize / 2.f ,sidePocketSize / 2.f });
                }
                else {
                    circleShape.setRadius(cornerPocketSize / 2.f);
                    circleShape.setOrigin({ cornerPocketSize / 2.f ,cornerPocketSize / 2.f });
                }

                if (holeTimeLeft[count] > 0.f) {
                    holeTimeLeft[count] = std::max(0.f, holeTimeLeft[count] - theRate);
                    circleShape.setFillColor(sf::Color::Green);
                }

                circleShape.setPosition(circles);
                window.draw(circleShape);

                count++;
            }

            // If Rotate Using Key Mode is on
            if (rotateArrowUsingKeys) {

                //std::cout << "rotateArrows On" << std::endl;

                sf::RectangleShape keysOnIndicator;
                keysOnIndicator.setFillColor(sf::Color::White);
                keysOnIndicator.setSize({ jj,jj });
                keysOnIndicator.setPosition({ ff - 3.f * jj, hh + jj * 1.6f });

                window.draw(keysOnIndicator);
            }

            // Draw Pool Balls
            for (int i = 0; i < animation.balls.size(); i++) {

                Ball& mainBall = animation.balls[i];

                sf::CircleShape whiteHighlight(ballSize / 2.f * 0.9f);
                whiteHighlight.setFillColor(sf::Color::Transparent);
                whiteHighlight.setOutlineThickness(1.5f);
                whiteHighlight.setPosition({ mainBall.position.x - ballSize / 2.f + 0.5f, mainBall.position.y - ballSize / 2.f + 0.5f });

                if (mainBall.velocity.x == 0.f && mainBall.velocity.y == 0.f && previousBallGroupSize == currentBallGroupSize) {
                    window.draw(mainBall.shape);

                    if (turnOnWhiteHighlight) {
                        window.draw(whiteHighlight);
                    }

                    continue;
                }

                StereographicProjection stP(*mainBall.ballImage, mainBall.projectionImage, ballSize / 2.f, mainBall.returnAngles, mainBall.rotAngles, mainBall.velocity, 0.003f);
                mainBall.rotAngles = mainBall.returnAngles;

                // Debug
                if (mainBall.velocity.x == 0.f && mainBall.velocity.y == 0.f) {
                    //std::cout << "originalAngles = (" << mainBall.rotAngles.x * (180.f / PI) << "," << mainBall.rotAngles.y * (180.f / PI) << ",";
                    //std::cout << mainBall.rotAngles.z * (180.f / PI) << ")" << std::endl;
                }

                mainBall.drawBall();
                mainBall.ballTexture->update(mainBall.projectionImage);


                mainBall.shape.setTexture(mainBall.ballTexture);
                mainBall.shape.setTextureRect(sf::IntRect(0.f, 0.f, ballSize, ballSize));

                window.draw(mainBall.shape);

                if (turnOnWhiteHighlight) {
                    window.draw(whiteHighlight);
                }

                // Draw halo on ball

                if (haloMode) {
                    sf::CircleShape halo(animation.balls[i].radius * 2.f);
                    halo.setOrigin({ animation.balls[i].radius * 2.f ,animation.balls[i].radius * 2.f });
                    halo.setPosition(animation.balls[i].position);
                    halo.setFillColor(sf::Color::Transparent);
                    halo.setOutlineThickness(2.f);

                    window.draw(halo);
                }

            }

            previousBallGroupSize = currentBallGroupSize;

        }

        // Draw Aim Mode Lines
        if (true) {

            // Detect if any of the balls are clicked on, and if so, which one

            bool allowAimMode = true;

            // Only allow aim Mode if all ball are not moving / aka all ball velocities are equal to 0
            for (auto a : animation.balls) {
                if (a.velocity.x != 0.f && a.velocity.y != 0.f) {

                    // Debug
                    //std::cout << "There's a ball still moving!" << std::endl;
                    gameStateLoadPause = true;
                    allowAimMode = false;
                    break;
                }
            }


            if (allowAimMode) {

                if (gameStateLoadPause) {
                    GameState someState(animation.balls);

                    gameStates.addGameState(someState); // Game States HERE
                    gameStateLoadPause = false;


                }

                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {

                    rightButtonDistanceMoved = 0.f;
                    firstRightClickPoint = { 0,0 };

                    int count = 0;

                    // Determine which pool ball was clicked
                    for (auto& ball : animation.balls) {

                        sf::Vector2i mouseCoordinates = sf::Mouse::getPosition(window);

                        if (ballWithAimModeOnIndex == count && mouseCoordinates != oldMouseCoordinates) {

                            // If the ball is stationary and clicked once, then the mouse is moved from the point of click origin
                            // and when clicked again, the ball will move in the direction opposite the mouse

                            float speedLimit = 1500.f;

                            float mouseSpeedX = 7.5f * -(float)(mouseCoordinates.x - ball.position.x);
                            float absoluteValMouseSpeedX = abs(mouseSpeedX);

                            float mouseSpeedY = 7.5f * -(float)(mouseCoordinates.y - ball.position.y);
                            float absoluteValMouseSpeedY = abs(mouseSpeedY);

                            ballWithAimModeOnIndex = -1;

                            oldMouseCoordinates = mouseCoordinates;
                            break;
                        }


                        if (ballWithAimModeOnIndex == -1 && ball.mouseInsideCircle(mouseCoordinates.x, mouseCoordinates.y)
                            && ball.velocity.x == 0.f && ball.velocity.y == 0.f) {

                            // If no balls are in aim Mode and the clicked ball is not moving (aka velocity is 0), then
                            // activate the ball's aim mode

                            // Debug
                            std::cout << "Aim Mode Activate On" << std::endl;

                            ball.aimMode = true;
                            ballWithAimModeOnIndex = count;
                            //pathTrackerBallNumber = ball.ballNumber;
                            pathTrackerBallNumber = -1;
                            pathTracker.clearAllVectors();

                            oldMouseCoordinates = mouseCoordinates;

                            break;
                        }

                        count++;
                    }
                }
                else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {

                    if (ballWithAimModeOnIndex != -1) {


                        for (auto shape : oldAimShapeLines) {
                            window.draw(shape);
                        }

                        for (auto shape : oldAimShapeCircles) {
                            window.draw(shape);
                        }


                        sf::Vector2i intMousePos = sf::Mouse::getPosition(window);
                        sf::Vector2f floatMousePos = { (float)intMousePos.x, (float)intMousePos.y };

                        if (firstRightClickPoint.x == -1 && firstRightClickPoint.y == -1) {
                            firstRightClickPoint = intMousePos;
                        }

                        rightButtonDistanceMoved = sqrtf(powf(floatMousePos.x - firstRightClickPoint.x, 2) + powf(floatMousePos.y - firstRightClickPoint.y, 2));


                        oldMouseCoordinates = intMousePos;

                        Ball& mainBall = animation.balls[ballWithAimModeOnIndex];

                        sf::Vector2f correctPositionVector;

                        float convertedAngleToRadians = (PI / 180.f) * startingAngle;

                        correctPositionVector = { cosf(convertedAngleToRadians), sinf(convertedAngleToRadians) };

                        CuePowerBar powerBar(mainBall, correctPositionVector, rightButtonDistanceMoved, 2.f, cuePosition, cueSize, true, maxVelocityMagnitude);

                        resolvedSpeedsAimMode = powerBar.returnSpeedVector();

                        window.draw(powerBar.cueDistanceMoved);
                        window.draw(powerBar.cue);
                    }

                }
                else { // if the right button mouse is released (or if no mouse clicks are made)

                    if (ballWithAimModeOnIndex != -1 && rightButtonDistanceMoved > 0.f) 
                    {
                        pathTrackerBallNumber = animation.balls[ballWithAimModeOnIndex].ballNumber;
                        animation.balls[ballWithAimModeOnIndex].velocity.x = resolvedSpeedsAimMode.x;
                        animation.balls[ballWithAimModeOnIndex].velocity.y = resolvedSpeedsAimMode.y;
                        ballWithAimModeOnIndex = -1;
                    }

                    rightButtonDistanceMoved = 0.f;
                    firstRightClickPoint = { -1,-1 };
                }

                // Draw the chosen ball's aim mode
                if (ballWithAimModeOnIndex != -1) {


                    sf::Vector2i intMousePos = sf::Mouse::getPosition(window);

                    sf::Vector2f floatMousePos = { (float)intMousePos.x, (float)intMousePos.y };
                    sf::Vector2f floatScreenSize = { (float)screenSize.x, (float)screenSize.y };



                    float oldMouseNewMouseDistance = sqrtf(powf(floatMousePos.x - oldMouseCoordinates.x, 2) + powf(floatMousePos.y - oldMouseCoordinates.y, 2));

                    if (!sf::Mouse::isButtonPressed(sf::Mouse::Right)) {

                        oldAimShapeLines.clear();
                        oldAimShapeCircles.clear();

                        Ball& mainBall = animation.balls[ballWithAimModeOnIndex];

                        sf::Vector2f normalizedBallMouseVector = { floatMousePos.x - mainBall.position.x, floatMousePos.y - mainBall.position.y };

                        sf::Vector2f correctPositionVector;

                        float convertedAngleToRadians;

                        if (rotateArrowUsingKeys) {
                            convertedAngleToRadians = (PI / 180.f) * startingAngle;
                            correctPositionVector = { -cos(convertedAngleToRadians), -sin(convertedAngleToRadians) };
                        }
                        else {

                            startingAngle = atan2(-normalizedBallMouseVector.y, -normalizedBallMouseVector.x) * (180.f / PI);
                            convertedAngleToRadians = (PI / 180.f) * startingAngle;
                            correctPositionVector = { -cos(convertedAngleToRadians), -sin(convertedAngleToRadians) };


                        }

                        AimShape aimShape(mainBall, animation.balls, correctPositionVector, floatScreenSize, currentAimLengthAddedLength, true, contactLineValues);


                        sf::Vertex aimLine[] = { aimShape.aimLine[0], aimShape.aimLine[1] };

                        for (int i = 0; i < 1; i++) {

                            std::vector<sf::Vector2f> line = { aimLine[0].position, aimLine[1].position };

                            float lineLength = sqrtf(powf(line[1].x - line[0].x, 2) + powf(line[1].y - line[0].y, 2));
                            float dotProduct = ((line[1].x - line[0].x) * 1.f + (line[1].y - line[0].y) * 0.f) / lineLength;
                            float crossProduct = ((line[1].y - line[0].y) * 1.f + (line[1].x - line[0].x) * 0.f) / lineLength;

                            float tanTheta = atan2f(crossProduct, dotProduct);
                            tanTheta *= (180.f / PI);

                            sf::RectangleShape rectLine;
                            rectLine.setSize({ lineLength, theThickness });
                            rectLine.setOrigin({ 0.f, theThickness / 2.f });
                            rectLine.setPosition(line[0]);
                            rectLine.setFillColor(sf::Color::White);
                            rectLine.rotate(tanTheta);

                            oldAimShapeLines.push_back(rectLine);

                            //window.draw(someLine, 2, sf::Lines);
                            window.draw(rectLine);
                        }

                        if (aimShape.circleOn) {

                            //sf::CircleShape firstVertex(10.f);
                            //firstVertex.setOrigin({ 10.f,10.f });
                            //firstVertex.setPosition(pathPredictor.pathPoints[0]);
                            //firstVertex.setFillColor(sf::Color::Red);

                            if (pathPredictorMode) {
                                float pathPredictorRadiusSize = 5.f;
                                PathPredictor pathPredictor(aimShape.targetBall, animation.balls, aimShape.aimNormalLine, contactTableLines, allTableCircles, sidePocketSize / 2.f, screenSize);
                                for (auto point : pathPredictor.pathPoints) {
                                    sf::CircleShape theVertex(pathPredictorRadiusSize);
                                    theVertex.setOrigin({ pathPredictorRadiusSize,pathPredictorRadiusSize });
                                    theVertex.setPosition(point);
                                    theVertex.setFillColor(sf::Color::Red);
                                    window.draw(theVertex);
                                }

                                for (auto rect : pathPredictor.theRectLines)
                                {
                                    window.draw(rect);
                                }

                            }


                            oldAimShapeCircles.push_back(aimShape.aimCircleProjection);

                            window.draw(aimShape.aimCircleProjection);

                            sf::Vertex aimNormalLine[] = { aimShape.aimNormalLine[0], aimShape.aimNormalLine[1] };
                            sf::Vertex aimTangentLine[] = { aimShape.aimTangentLine[0], aimShape.aimTangentLine[1] };

                            std::vector<sf::Vector2f> aimNormalPositions = { aimShape.aimNormalLine[0].position, aimShape.aimNormalLine[1].position };
                            std::vector<sf::Vector2f> aimTangentPositions = { aimShape.aimTangentLine[0].position, aimShape.aimTangentLine[1].position };

                            std::vector<std::vector<sf::Vector2f>> normalAndTangentPositions;
                            normalAndTangentPositions.push_back(aimNormalPositions);
                            normalAndTangentPositions.push_back(aimTangentPositions);

                            for (int i = 0; i < normalAndTangentPositions.size(); i++) {

                                std::vector<sf::Vector2f> line = normalAndTangentPositions[i];

                                float lineLength = sqrtf(powf(line[1].x - line[0].x, 2) + powf(line[1].y - line[0].y, 2));
                                float dotProduct = ((line[1].x - line[0].x) * 1.f + (line[1].y - line[0].y) * 0.f) / lineLength;
                                float crossProduct = ((line[1].y - line[0].y) * 1.f + (line[1].x - line[0].x) * 0.f) / lineLength;

                                float tanTheta = atan2f(crossProduct, dotProduct);
                                tanTheta *= (180.f / PI);

                                sf::RectangleShape rectLine;
                                rectLine.setSize({ lineLength, theThickness });
                                rectLine.setOrigin({ 0.f, theThickness / 2.f });
                                rectLine.setPosition(line[0]);
                                rectLine.setFillColor(sf::Color::White);
                                rectLine.rotate(tanTheta);

                                oldAimShapeLines.push_back(rectLine);

                                window.draw(rectLine);
                            }

                        }
                    }
                    else {

                    }

                    oldMouseCoordinates = intMousePos;


                }
            }

        }

        if (pathTrackerMode && pathTrackerBallNumber != -1) {
            for (auto someBall : animation.balls)
            {
                if (pathTrackerBallNumber == someBall.ballNumber)
                {
                    pathTracker.addPosition(someBall.position);
                    for (auto image : pathTracker.pathPositions)
                    {
                        window.draw(image.second);
                    }
                }
            }
        }

        window.display();
    }

    return 0;
}