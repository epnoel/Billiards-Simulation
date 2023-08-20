#include "AimShape.h"

AimShape::AimShape() {
    circleOn = false;
    slope = 0;
    inverseSlope = 0;
}

AimShape::AimShape(Ball ball1, std::vector<Ball> balls, sf::Vector2f normalizedBallMouseVector, sf::Vector2f screenSize, float addedLength, bool usingOtherWallLines,
    std::vector<std::vector<float>> contactLineValues) {

    circleOn = false;

    slope = normalizedBallMouseVector.y / normalizedBallMouseVector.x;
    inverseSlope = -1.f / slope;

    //if (abs(normalizedBallMouseVector.x) <= 0.012f)
    //{
    //    std::cout << "Mouse Position delta X = 0" << std::endl;
    //    std::cout << "slope = " << slope << std::endl;
    //}

    //std::cout << "normalizedBallMouseVector.x = " << normalizedBallMouseVector.x << std::endl;

    double yIntercept = (float)ball1.position.y - slope * (float)ball1.position.x;

    Ball ball2;

    sf::Vector2f aimLineEndPoints;

    aimLineEndPoints = calculateAimLineEndPoints(ball1, normalizedBallMouseVector, screenSize);

    std::vector<sf::Vector2f> lineBoundingBox = { {std::min(ball1.position.x, aimLineEndPoints.x), std::min(ball1.position.y, aimLineEndPoints.y) },
                                                  {std::max(ball1.position.x, aimLineEndPoints.x), std::max(ball1.position.y, aimLineEndPoints.y) } }; // get the bounding box of line

    ball2 = getCorrectBallAlongLine(balls, normalizedBallMouseVector, screenSize, ball1, true, contactLineValues);
    targetBall = ball2;

    double a, b, c;
    a = 1.0 + pow(slope, 2);
    b = 2.0 * (slope * (yIntercept - ball2.position.y) - ball2.position.x);
    c = pow(ball2.position.x, 2) + pow(yIntercept - ball2.position.y, 2) - pow(ball2.radius + ball1.radius, 2); // added ball1.radius

    double remainder = pow(b, 2) - 4 * a * c;


    if (ball1.position != ball2.position) {
        circleOn = true;
    }
    else {
        // Called when there are no balls in the aim line's sight
        circleOn = false;
    }

    if (circleOn) {
        float x1, x2, yEnd;
        sf::Vector2f normalEnds, tangentEnds;

        float straightSlope = (ball2.position.y - ball1.position.y) / (ball2.position.x - ball1.position.x);


        x1 = (float) ( (-b - sqrt(remainder)) / (2.0 * a) );
        x2 = (float) ( (-b + sqrt(remainder)) / (2.0 * a) );

        float yEnd1 = (float)slope * x1 + (float)yIntercept;
        float yEnd2 = (float)slope * x2 + (float)yIntercept;

        float distance1 = sqrtf(powf(x1 - ball1.position.x, 2) + powf(yEnd1 - ball1.position.y, 2));
        float distance2 = sqrtf(powf(x2 - ball1.position.x, 2) + powf(yEnd2 - ball1.position.y, 2));


        sf::Vector2f theXY;

        if (distance1 < distance2) {
            theXY = { x1,yEnd1 };
        }
        else {
            theXY = { x2, yEnd2 };
        }

        float sinTheta;

        std::vector<sf::Vector2f> rotatedCoords;

        if (abs(normalizedBallMouseVector.x) <= 0.012f)
        {
            std::cout << "ball1.position = (" << ball1.position.x << "," << ball1.position.y << ")" << std::endl;
            std::cout << "theXY = (" << theXY.x << "," << theXY.y << ")" << std::endl;
        }


        yEnd = theXY.y;
        sf::Vector2f lineEndPoints = theXY;
        aimLine = { sf::Vertex(ball1.position), sf::Vertex(lineEndPoints) };

        aimCircleProjection.setRadius(ball1.radius);
        aimCircleProjection.setOrigin({ ball1.radius, ball1.radius });
        aimCircleProjection.setOutlineThickness((5.f / 2024.f) * screenSize.x);
        aimCircleProjection.setOutlineColor(sf::Color::White);
        aimCircleProjection.setFillColor(sf::Color::Transparent);
        aimCircleProjection.setPosition(lineEndPoints);


        float aimLineMagnitude = sqrtf(powf(aimLine[1].position.x - aimLine[0].position.x, 2) + powf(aimLine[1].position.y - aimLine[0].position.y, 2));
        sf::Vector2f normalizedAimLineVector = { (aimLine[1].position.x - aimLine[0].position.x) / aimLineMagnitude, (aimLine[1].position.y - aimLine[0].position.y) / aimLineMagnitude };
        sf::Vector2f ball1ToBall2Vector = { ball2.position.x - ball1.position.x, ball2.position.y - ball1.position.y };
        float ball1ToBall2Magnitude = sqrtf(powf(ball1ToBall2Vector.x, 2) + powf(ball1ToBall2Vector.y, 2));

        ball1ToBall2Vector.x /= ball1ToBall2Magnitude;
        ball1ToBall2Vector.y /= ball1ToBall2Magnitude;

        sf::Vector2f normalBall1Ball2 = { -ball1ToBall2Vector.y, ball1ToBall2Vector.x };
        sf::Vector2f locationNormalMax = { ball2.position.x + normalBall1Ball2.x * (ball1.radius + ball2.radius), ball2.position.y + normalBall1Ball2.y * (ball1.radius + ball2.radius) };

        sf::Vector2f ball1ToNormalMax = { locationNormalMax.x - ball1.position.x, locationNormalMax.y - ball1.position.y };
        float ballToNormalMaxMagnitude = sqrtf(powf(ball1ToNormalMax.x, 2) + powf(ball1ToNormalMax.y, 2));
        ball1ToNormalMax.x /= ballToNormalMaxMagnitude;
        ball1ToNormalMax.y /= ballToNormalMaxMagnitude;

        float cosOfTheAngle = ball1ToBall2Vector.x * ball1ToNormalMax.x + ball1ToBall2Vector.y * ball1ToNormalMax.y;

        float theAngle = acos(cosOfTheAngle);
        float angleMultipleFactor = ((PI / 2.f) / theAngle);

        sinTheta = normalizedAimLineVector.x * ball1ToBall2Vector.y - normalizedAimLineVector.y * ball1ToBall2Vector.x;

        float sinAngle = asin(sinTheta);

        float realCosTheta = cos(angleMultipleFactor * sinAngle);
        float realSinTheta = sin(angleMultipleFactor * sinAngle);


        sf::Vector2f aimCircleToBall2Vector = { ball2.position.x - aimCircleProjection.getPosition().x, ball2.position.y - aimCircleProjection.getPosition().y };
        float aimCircleToBall2Magntiude = sqrtf(powf(aimCircleToBall2Vector.x, 2) + powf(aimCircleToBall2Vector.y, 2));
        aimCircleToBall2Vector.x /= aimCircleToBall2Magntiude;
        aimCircleToBall2Vector.y /= aimCircleToBall2Magntiude;

        sf::Vector2f aimCircleToBall2TangentVector = { -aimCircleToBall2Vector.y, aimCircleToBall2Vector.x };

        normalEnds = calculateNormalLineEndPoints(ball1, ball2, 50.f + addedLength, realCosTheta, aimCircleToBall2Vector);

        tangentEnds = calculateTangentLineEndPoints(ball2, 50.f + addedLength, straightSlope, realSinTheta, aimCircleToBall2TangentVector);

        aimNormalLine = { sf::Vertex(aimCircleProjection.getPosition()), sf::Vertex(normalEnds) };
        aimTangentLine = { sf::Vertex(aimCircleProjection.getPosition()), sf::Vertex(tangentEnds) };

    }
    //else if (circleOn && normalizedBallMouseVector.x == 0.f) {
    //    sf::Vector2f ball1ToBall2 = { ball2.position.x - ball1.position.x,ball2.position.y - ball1.position.y };
    //    if (ball1ToBall2.y > 0.f) {
    //        ball1ToBall2.y -= ball1.radius;
    //    }
    //    else {
    //        ball1ToBall2.y += ball1.radius;
    //    }
    //    sf::Vector2f normalEnds, tangentEnds;
    //    normalEnds
    //}
    else {

        sf::Vector2f screenSizef = { (float)screenSize.x, (float)screenSize.y };

        sf::Vector2f lineEndPoints = calculateAimLineEndPoints(ball1, normalizedBallMouseVector, screenSizef, usingOtherWallLines, contactLineValues);

        aimLine = { sf::Vertex(ball1.position), lineEndPoints };
    }


}

int AimShape::getClosestBallIndex(std::vector<Ball> balls, sf::Vector2f normalizedBallMouseVector, Ball ball1) {
    int closestBallIndex = -1;
    float shortestDist = (float)INT_MAX;
    int count = -1;

    for (Ball ball : balls) {

        count++;

        if (ball.position == ball1.position) {
            continue;
        }

        if (false) {
            continue;
        }

        float dist = sqrtf(powf(ball1.position.x - ball.position.x, 2) + powf(ball1.position.y - ball.position.y, 2));

        if (shortestDist > dist) {
            shortestDist = dist;
            closestBallIndex = count;
        }

    }

    return closestBallIndex;

}

Ball AimShape::getClosestBall(std::vector<Ball> balls, Ball ball1) {
    float shortestDist = (float)INT_MAX;
    Ball closestBall = ball1;

    for (Ball ball : balls) {

        float dist = sqrtf(powf(ball1.position.x - ball.position.x, 2) + powf(ball1.position.y - ball.position.y, 2));

        if (shortestDist > dist) {
            shortestDist = dist;
            closestBall = ball;
        }

    }

    return closestBall;
}

Ball AimShape::getCorrectBallAlongLine(std::vector<Ball> balls, sf::Vector2f normalizedBallMouseVector, sf::Vector2f screenSize, Ball ball1, bool usingOtherWallLines,
    std::vector<std::vector<float>> contactLineValues) {

    std::vector<Ball> ballsAlongLine; // store all of the balls that intersect with the aimLine
    double yIntercept = (float)ball1.position.y - (float)slope * (float)ball1.position.x;
    sf::Vector2f aimLineEndPoints;

    aimLineEndPoints = calculateAimLineEndPoints(ball1, normalizedBallMouseVector, screenSize);

    std::vector<sf::Vector2f> lineBoundingBox = { {std::min(ball1.position.x, aimLineEndPoints.x), std::min(ball1.position.y, aimLineEndPoints.y) },
                                                  {std::max(ball1.position.x, aimLineEndPoints.x), std::max(ball1.position.y, aimLineEndPoints.y) } }; // get the bounding box of line

    int ballCount = -1;

    for (auto i : balls) {


        if (i.position == ball1.position) {
            continue;
        }

        ballCount++;

        //if (abs(normalizedBallMouseVector.x) == 0.f) {
        //    if (abs(i.position.x - ball1.position.x) < ball1.radius) {
        //        if (i.position.y >= lineBoundingBox[0].y && i.position.y <= lineBoundingBox[1].y) {
        //            ballsAlongLine.push_back(i);
        //            continue;
        //        }
        //    }
        //}

        double a, b, c;

        a = 1.0 + pow(slope, 2);
        b = 2.0 * (slope * (double) (yIntercept - i.position.y) - (double) i.position.x);
        c = pow((double)i.position.x, 2) + pow(yIntercept - (double) i.position.y, 2) - pow((double)(i.radius + ball1.radius), 2);


        double remainder = pow(b, 2) - 4 * a * c;

        if (abs(slope) > 130.f)
        {
            if (ballCount == 1) {
                //std::cout << "Place:" << std::endl;
                //std::cout << "slope = " << slope << "; yIntercept = " << yIntercept << "; remainder = " << remainder << std::endl;
                //std::cout << "i.radius + ball1.radius = " << ball1.radius + i.radius << std::endl;
                //std::cout << "a = " << a << "; b = " << b << "; c = " << c << std::endl;
                //std::cout << "lineBoundingBox = (" << lineBoundingBox[0].x << "," << lineBoundingBox[0].y << ") and ";
                //std::cout << "(" << lineBoundingBox[1].x << "," << lineBoundingBox[1].y << ")" << std::endl;
                //std::cout << "ball2.position = (" << i.position.x << "," << i.position.y << ")" << std::endl;
                //std::cout << std::endl;
            }
        }


        if (remainder >= 0.f) {

            float x1 = (float) ( (-b - sqrt(remainder)) / (2.0 * a) );
            float y1 = (float)slope * x1 + (float)yIntercept;

            float x2 = (float) ( (-b + sqrt(remainder)) / (2.0 * a) );
            float y2 = (float)slope * x2 + (float)yIntercept;

            //double distance1, distance2;
            //distance1 = sqrtf(powf(x1 - ball1.position.x, 2) + powf(y1 - ball1.position.y, 2));
            //distance2 = sqrtf(powf(x2 - ball1.position.x, 2) + powf(y2 - ball1.position.y, 2));


            if (abs(slope) > 130.f)
            {
                //std::cout << "Remainder is greater than or equal to 0!" << std::endl;
                //std::cout << "slope = " << slope << "; remainder = " << remainder << std::endl;
                ////std::cout << "a = " << a << "; b = " << b << "; c = " << c << std::endl;
                //std::cout << "lineBoundingBox = (" << lineBoundingBox[0].x << "," << lineBoundingBox[0].y << ") and ";
                //std::cout << "(" << lineBoundingBox[1].x << "," << lineBoundingBox[1].y << ")" << std::endl;
                //std::cout << "ball2.position = (" << i.position.x << "," << i.position.y << ")" << std::endl;
                //std::cout << "(x1,y1) = (" << x1 << "," << y1 << ")" << std::endl;
                //std::cout << "(x2,y2) = (" << x2 << "," << y2 << ")" << std::endl;
                //std::cout << "distance1 = " << distance1 << std::endl;
                //std::cout << "distance2 = " << distance2 << std::endl;
                std::cout << std::endl;
            }


            if ((x1 >= lineBoundingBox[0].x && x1 <= lineBoundingBox[1].x
                && y1 >= lineBoundingBox[0].y && y1 <= lineBoundingBox[1].y)
                || (x2 >= lineBoundingBox[0].x && x2 <= lineBoundingBox[1].x
                    && y2 >= lineBoundingBox[0].y && y2 <= lineBoundingBox[1].y))
            {
                ballsAlongLine.push_back(i);
            }

        }
    }


    if (ballsAlongLine.empty()) {
        return ball1;
    }

    return getClosestBall(ballsAlongLine, ball1);
}

sf::Vector2f AimShape::calculateNormalLineEndPoints(Ball ball1, Ball ball2, float addedDistance, float cosTheta, sf::Vector2f normalCoords) {
    float NX2, NY2;

    float d = (aimCircleProjection.getRadius() + ball2.radius + addedDistance) * abs(cosTheta);

    sf::Vector2f aimCirclePosition = aimCircleProjection.getPosition();

    NX2 = normalCoords.x * d + aimCirclePosition.x;
    NY2 = normalCoords.y * d + aimCirclePosition.y;

    return { NX2, NY2 };
}

sf::Vector2f AimShape::calculateTangentLineEndPoints(Ball ball2, float addedDistance, float straightSlope, float sinTheta, sf::Vector2f tangentCoords) {
    float TX2, TY2;
    float d = (aimCircleProjection.getRadius() + ball2.radius + addedDistance) * -sinTheta;

    sf::Vector2f aimCirclePosition = aimCircleProjection.getPosition();

    TX2 = tangentCoords.x * d + aimCirclePosition.x;
    TY2 = tangentCoords.y * d + aimCirclePosition.y;

    return { TX2, TY2 };
}

sf::Vector2f AimShape::calculateAimLineEndPoints(Ball ball1, sf::Vector2f normalizedBallMouseVector, sf::Vector2f screenSize, bool usingOtherWallLines,
    std::vector<std::vector<float>> contactLineValues) {

    float yIntercept = (float)ball1.position.y - (float)slope * (float)ball1.position.x;

    if (!usingOtherWallLines) {

        float endX;

        if (normalizedBallMouseVector.x == 0.f) {
            if (normalizedBallMouseVector.y < 0.f) {
                return { ball1.position.x, screenSize.y };
            }
            else {
                return { ball1.position.x, 0.f };
            }
        }
        else if (normalizedBallMouseVector.y == 0.f) {
            if (normalizedBallMouseVector.x < 0.f) {
                return { screenSize.x, ball1.position.y };
            }
            else {
                return { 0.f, ball1.position.y };
            }
        }
        else if (normalizedBallMouseVector.x < 0.f && normalizedBallMouseVector.y < 0.f) {
            endX = (screenSize.y - yIntercept) / (float)slope;

            if (endX < screenSize.x) {
                return { endX, (float)screenSize.y };
            }
            else {
                return { screenSize.x, (float)slope * screenSize.x + yIntercept };
            }
        }
        else if (normalizedBallMouseVector.x < 0.f && normalizedBallMouseVector.y > 0.f) {
            endX = (0.f - yIntercept) / (float)slope;

            if (endX < screenSize.x) {
                return { endX, 0.f };
                ;
            }
            else {
                return { screenSize.x, (float)slope * screenSize.x + yIntercept };
            }
        }
        else if (normalizedBallMouseVector.x > 0.f && normalizedBallMouseVector.y < 0.f) {
            endX = endX = (screenSize.y - yIntercept) / (float)slope;

            if (endX > 0.f) {
                return { endX, screenSize.y };
            }
            else {
                return { 0.f, yIntercept };
            }
        }
        else /*if (mousePos.x > ball1.position.x && mousePos.y > ball1.position.y)*/ {
            endX = (0 - yIntercept) / (float)slope;

            if (endX > 0.f) {
                return { endX, 0.f };
            }
            else {
                return { 0.f, yIntercept };
            }
        }
    }
    else {

        float shortestDistance = (float)INT_MAX;

        sf::Vector2f regularAimLineEndPoints = calculateAimLineEndPoints(ball1, normalizedBallMouseVector, screenSize);

        sf::Vector2f endPoints = regularAimLineEndPoints;

        std::vector<sf::Vector2f> boundingBox = { {std::min(ball1.position.x,regularAimLineEndPoints.x), std::min(ball1.position.y, regularAimLineEndPoints.y)},
                                                  {std::max(ball1.position.x,regularAimLineEndPoints.x), std::max(ball1.position.y, regularAimLineEndPoints.y)} };

        for (auto vals : contactLineValues) {

            float x1, y1;

            if (normalizedBallMouseVector.x == 0.f) {


                x1 = ball1.position.x;
                y1 = ball1.position.y;

                if (x1 >= vals[2] && x1 <= vals[4]) {
                    float theDistance = sqrtf(powf(x1 - ball1.position.x, 2) + powf(y1 - ball1.position.y, 2));
                    y1 = vals[0] * x1 + vals[1];

                    if (theDistance < shortestDistance) {
                        shortestDistance = theDistance;
                        endPoints = { x1, y1 };
                    }
                }

                continue;
            }

            // If tl.x & br.x are equal
            if (vals[2] == vals[4]) {
                x1 = vals[2];
                y1 = (float)slope * x1 + yIntercept;
            }
            else {
                x1 = (vals[1] - yIntercept) / ((float)slope - vals[0]);
                y1 = (float)slope * x1 + yIntercept;

            }

            if ((x1 >= vals[2] - 0.01f && x1 <= vals[4] + 0.01f
                && y1 >= vals[3] - 0.01f && y1 <= vals[5] + 0.01f))
            {

                if (x1 >= boundingBox[0].x && x1 <= boundingBox[1].x
                    && y1 >= boundingBox[0].y && y1 <= boundingBox[1].y) {

                }
                else {

                    continue;
                }

                float theDistance = sqrtf(powf(x1 - ball1.position.x, 2) + powf(y1 - ball1.position.y, 2));

                if (theDistance < shortestDistance) {
                    shortestDistance = theDistance;
                    endPoints = { x1, y1 };
                }
            }

        }

        return endPoints;
    }
}