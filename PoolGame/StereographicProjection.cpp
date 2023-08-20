#include "StereographicProjection.h"

StereographicProjection::StereographicProjection(sf::Image baseImage, sf::Image& editImage, int circleRadius, sf::Vector3f& returnAngles,
                                                sf::Vector3f originalAngles, sf::Vector2f velocityVector, float rotationFactor)
{

    if (velocityVector.x == 0.f && velocityVector.y == 0.f) {
        convertToStereographicImage(baseImage, editImage, circleRadius, returnAngles, originalAngles);
    }
    else {
        rollBallAlongVelocityVector(baseImage, editImage, circleRadius, returnAngles, originalAngles, velocityVector, rotationFactor);
    }
}

bool StereographicProjection::convertToStereographicImage(sf::Image originalImage, sf::Image& editImage, int circleRadius, sf::Vector3f& returnMidPointAngles,
                                                            sf::Vector3f originalAngles, sf::Vector3f angles)
{

    float stepSize;
    stepSize = 1.f / (float)circleRadius;

    // Assume that editImage.size() is set and is equal to the same size as orginalImage

    float theHeight = (float)originalImage.getSize().y;
    float theWidth = (float)originalImage.getSize().x;


    float sinA0, cosA0, sinB0, cosB0, sinY0, cosY0;

    sinA0 = sinf(originalAngles.z);
    cosA0 = cosf(originalAngles.z);

    sinB0 = sinf(originalAngles.y);
    cosB0 = cosf(originalAngles.y);

    sinY0 = sinf(originalAngles.x);
    cosY0 = cosf(originalAngles.x);


    float sinA, cosA, sinB, cosB, sinY, cosY;

    sinA = sinf(angles.z);
    cosA = cosf(angles.z);

    sinB = sinf(angles.y);
    cosB = cosf(angles.y);

    sinY = sinf(angles.x);
    cosY = cosf(angles.x);

    std::vector<std::vector<float>> rotationMatrix(3, std::vector<float>(3, 0.f));
    rotationMatrix[0][0] = cosA0 * cosB0;
    rotationMatrix[0][1] = cosA0 * sinB0 * sinY0 - sinA0 * cosY0;
    rotationMatrix[0][2] = cosA0 * sinB0 * cosY0 + sinA0 * sinY0;
    rotationMatrix[1][0] = sinA0 * cosB0;
    rotationMatrix[1][1] = sinA0 * sinB0 * sinY0 + cosA0 * cosY0;
    rotationMatrix[1][2] = sinA0 * sinB0 * cosY0 - cosA0 * sinY0;
    rotationMatrix[2][0] = -sinB0;
    rotationMatrix[2][1] = cosB0 * sinY0;
    rotationMatrix[2][2] = cosB0 * cosY0;

    std::vector<std::vector<float>> rotationMatrix2(3, std::vector<float>(3, 0.f));
    rotationMatrix2[0][0] = cosA * cosB;
    rotationMatrix2[0][1] = cosA * sinB * sinY - sinA * cosY;
    rotationMatrix2[0][2] = cosA * sinB * cosY + sinA * sinY;
    rotationMatrix2[1][0] = sinA * cosB;
    rotationMatrix2[1][1] = sinA * sinB * sinY + cosA * cosY;
    rotationMatrix2[1][2] = sinA * sinB * cosY - cosA * sinY;
    rotationMatrix2[2][0] = -sinB;
    rotationMatrix2[2][1] = cosB * sinY;
    rotationMatrix2[2][2] = cosB * cosY;

    std::vector<std::vector<float>> rotationMatrixCombined(3, std::vector<float>(3, 0.f));
    rotationMatrixCombined[0][0] = rotationMatrix[0][0] * rotationMatrix2[0][0] + rotationMatrix[0][1] * rotationMatrix2[1][0] + rotationMatrix[0][2] * rotationMatrix2[2][0];
    rotationMatrixCombined[0][1] = rotationMatrix[0][0] * rotationMatrix2[0][1] + rotationMatrix[0][1] * rotationMatrix2[1][1] + rotationMatrix[0][2] * rotationMatrix2[2][1];
    rotationMatrixCombined[0][2] = rotationMatrix[0][0] * rotationMatrix2[0][2] + rotationMatrix[0][1] * rotationMatrix2[1][2] + rotationMatrix[0][2] * rotationMatrix2[2][2];
    rotationMatrixCombined[1][0] = rotationMatrix[1][0] * rotationMatrix2[0][0] + rotationMatrix[1][1] * rotationMatrix2[1][0] + rotationMatrix[1][2] * rotationMatrix2[2][0];
    rotationMatrixCombined[1][1] = rotationMatrix[1][0] * rotationMatrix2[0][1] + rotationMatrix[1][1] * rotationMatrix2[1][1] + rotationMatrix[1][2] * rotationMatrix2[2][1];
    rotationMatrixCombined[1][2] = rotationMatrix[1][0] * rotationMatrix2[0][2] + rotationMatrix[1][1] * rotationMatrix2[1][2] + rotationMatrix[1][2] * rotationMatrix2[2][2];
    rotationMatrixCombined[2][0] = rotationMatrix[2][0] * rotationMatrix2[0][0] + rotationMatrix[2][1] * rotationMatrix2[1][0] + rotationMatrix[2][2] * rotationMatrix2[2][0];
    rotationMatrixCombined[2][1] = rotationMatrix[2][0] * rotationMatrix2[0][1] + rotationMatrix[2][1] * rotationMatrix2[1][1] + rotationMatrix[2][2] * rotationMatrix2[2][1];
    rotationMatrixCombined[2][2] = rotationMatrix[2][0] * rotationMatrix2[0][2] + rotationMatrix[2][1] * rotationMatrix2[1][2] + rotationMatrix[2][2] * rotationMatrix2[2][2];


    float xRot, yRot, zRot;
    xRot = atan2f(rotationMatrixCombined[2][1], rotationMatrixCombined[2][2]);
    zRot = atan2f(rotationMatrixCombined[1][0], rotationMatrixCombined[0][0]);

    if (cosf(zRot) == 0.f) {
        yRot = atan2(-rotationMatrixCombined[2][0], rotationMatrixCombined[1][0] / sinf(zRot));
    }
    else {
        yRot = atan2(-rotationMatrixCombined[2][0], rotationMatrixCombined[0][0] / cosf(zRot));
    }

    //std::cout << "xRot = " << xRot * (180.f / 3.14159f) << "; yRot = " << yRot * (180.f / 3.14159f) << "; zRot = " << zRot * (180.f / 3.14159f) << std::endl;
    //std::cout << "originalAngles = (" << originalAngles.x * (180.f / 3.14159f) << "," << originalAngles.y * (180.f / 3.14159f) << ",";
    //std::cout << originalAngles.z * (180.f / 3.14159f) << ")" << std::endl;
    returnMidPointAngles = { xRot,yRot,zRot };


    int rowCount, columnCount;
    rowCount = 0;
    columnCount = 0;

    int times = 0;

    // Debug
    //std::cout << "baseImage.size() = (" << baseImage.getSize().x << "," << baseImage.getSize().y << ")" << std::endl;

    for (float i = 1.f - stepSize; i >= -1.f; i -= stepSize) {

        columnCount = 0;

        for (float j = -1.f; j <= 1.f; j += stepSize) {
            float distanceFromMiddle;
            distanceFromMiddle = sqrtf(powf(i, 2) + powf(j, 2));

            if (distanceFromMiddle <= 1.f) {
                float x, y, z;
                x = j;
                y = i;
                z = sqrtf(1.f - powf(x, 2) - powf(y, 2));

                float newX0, newY0, newZ0;

                newX0 = x * rotationMatrixCombined[0][0] + y * rotationMatrixCombined[0][1] + z * rotationMatrixCombined[0][2];
                newY0 = x * rotationMatrixCombined[1][0] + y * rotationMatrixCombined[1][1] + z * rotationMatrixCombined[1][2];
                newZ0 = x * rotationMatrixCombined[2][0] + y * rotationMatrixCombined[2][1] + z * rotationMatrixCombined[2][2];



                if (abs(newX0) < 0.01f) {
                    newX0 = 0.f;
                }

                if (abs(newY0) < 0.01f) {
                    newY0 = 0.f;
                }

                if (abs(newZ0) < 0.01f) {
                    newZ0 = 0.f;
                }




                float u, v;
                u = atan2f(newY0, newX0) / PI;
                v = (2.f * -asin(newZ0)) / PI;

                sf::Vector2f normalizedCoords = { u, v };
                sf::Vector2f pixelCoords = convertToRegularCoords(normalizedCoords, (int)(theHeight - 1.f), (int)(theWidth - 1.f));
                sf::Color thePixel = originalImage.getPixel(pixelCoords.x, pixelCoords.y);
                sf::Vector2i targetPixelLocation = { rowCount, columnCount };
                editImage.setPixel(targetPixelLocation.y, targetPixelLocation.x, thePixel);

                if (pixelCoords.x >= theWidth || pixelCoords.y >= theHeight || pixelCoords.x < 0.f || pixelCoords.y < 0.f) {
                    std::cout << "normalizedCoords = (" << normalizedCoords.x << "," << normalizedCoords.y << "); ";
                    //std::cout << "actualPixel = (" << actualPixel.x << "," << actualPixel.y << ")" << std::endl;
                    std::cout << "(u,v) = (" << u << "," << v << ")" << std::endl;
                    std::cout << "x = " << x << ", y = " << y << ", z = " << z << std::endl;
                    return false;
                }

            }
            columnCount++;
        }
        rowCount++;
    }

    return true;

}

sf::Vector2f StereographicProjection::convertToRegularCoords(sf::Vector2f normalizedVal, int height, int width) {
    float fHeight = (float)height;
    float fWidth = (float)width;

    float xPart, yPart;
    xPart = ((normalizedVal.x + 1.f) * fWidth) / 2.f;
    yPart = ((normalizedVal.y + 1.f) * fHeight) / 2.f;

    return { xPart, yPart };
}

bool StereographicProjection::rollBallAlongVelocityVector(sf::Image originalImage, sf::Image& editImage, int circleRadius, sf::Vector3f& returnAngles,
                                                            sf::Vector3f originalAngles, sf::Vector2f velocityVector, float rotationFactor)
{
    if (velocityVector.x == 0.f && velocityVector.y == 0.f) {
        return true;
    }


    float stepSize;
    stepSize = 1.f / (float)circleRadius;

    // Assume that editImage.size() is set and is equal to the same size as orginalImage

    float theHeight = (float)originalImage.getSize().y;
    float theWidth = (float)originalImage.getSize().x;


    float sinA0, cosA0, sinB0, cosB0, sinY0, cosY0;

    sinA0 = sinf(originalAngles.z);
    cosA0 = cosf(originalAngles.z);

    sinB0 = sinf(originalAngles.y);
    cosB0 = cosf(originalAngles.y);

    sinY0 = sinf(originalAngles.x);
    cosY0 = cosf(originalAngles.x);

    sf::Vector2f unitVelocity;
    float velocityMagnitude = sqrtf(powf(velocityVector.x, 2) + powf(velocityVector.y, 2));
    unitVelocity = { velocityVector.x / velocityMagnitude, velocityVector.y / velocityMagnitude };

    float theta = atan2(-unitVelocity.y, unitVelocity.x); // -y b/c SFML y-axis is flipped
    float phi = theta - PI / 2.f;
    float cosPhi, sinPhi;
    cosPhi = cosf(phi);
    sinPhi = sinf(phi);

    sf::Vector3f axisVector;
    axisVector = { cosPhi, sinPhi, 0.f };

    //std::cout << "cosPhi = " << cosPhi << "; sinPhi = " << sinPhi << std::endl;

    float rotation;
    rotation = rotationFactor * velocityMagnitude;

    float cosRot, sinRot;
    cosRot = cosf(rotation);
    sinRot = sinf(rotation);

    std::vector<std::vector<float>> rotationAxisMatrix(3, std::vector<float>(3, 0.f));
    rotationAxisMatrix[0][0] = cosRot + powf(axisVector.x, 2) * (1.f - cosRot);
    rotationAxisMatrix[0][1] = axisVector.x * axisVector.y * (1.f - cosRot) - axisVector.z * sinRot;
    rotationAxisMatrix[0][2] = axisVector.x * axisVector.z * (1.f - cosRot) + axisVector.y * sinRot;
    rotationAxisMatrix[1][0] = axisVector.y * axisVector.x * (1.f - cosRot) + axisVector.z * sinRot;
    rotationAxisMatrix[1][1] = cosRot + powf(axisVector.y, 2) * (1.f - cosRot);
    rotationAxisMatrix[1][2] = axisVector.y * axisVector.z * (1.f - cosRot) - axisVector.x * sinRot;
    rotationAxisMatrix[2][0] = axisVector.z * axisVector.x * (1.f - cosRot) - axisVector.y * sinRot;
    rotationAxisMatrix[2][1] = axisVector.z * axisVector.y * (1.f - cosRot) + axisVector.x * sinRot;
    rotationAxisMatrix[2][2] = cosRot + powf(axisVector.z, 2) * (1.f - cosRot);


    std::vector<std::vector<float>> rotationMatrix(3, std::vector<float>(3, 0.f));
    rotationMatrix[0][0] = cosA0 * cosB0;
    rotationMatrix[0][1] = cosA0 * sinB0 * sinY0 - sinA0 * cosY0;
    rotationMatrix[0][2] = cosA0 * sinB0 * cosY0 + sinA0 * sinY0;
    rotationMatrix[1][0] = sinA0 * cosB0;
    rotationMatrix[1][1] = sinA0 * sinB0 * sinY0 + cosA0 * cosY0;
    rotationMatrix[1][2] = sinA0 * sinB0 * cosY0 - cosA0 * sinY0;
    rotationMatrix[2][0] = -sinB0;
    rotationMatrix[2][1] = cosB0 * sinY0;
    rotationMatrix[2][2] = cosB0 * cosY0;


    std::vector<std::vector<float>> rotationMatrixCombined(3, std::vector<float>(3, 0.f));
    rotationMatrixCombined[0][0] = rotationMatrix[0][0] * rotationAxisMatrix[0][0] + rotationMatrix[0][1] * rotationAxisMatrix[1][0] + rotationMatrix[0][2] * rotationAxisMatrix[2][0];
    rotationMatrixCombined[0][1] = rotationMatrix[0][0] * rotationAxisMatrix[0][1] + rotationMatrix[0][1] * rotationAxisMatrix[1][1] + rotationMatrix[0][2] * rotationAxisMatrix[2][1];
    rotationMatrixCombined[0][2] = rotationMatrix[0][0] * rotationAxisMatrix[0][2] + rotationMatrix[0][1] * rotationAxisMatrix[1][2] + rotationMatrix[0][2] * rotationAxisMatrix[2][2];
    rotationMatrixCombined[1][0] = rotationMatrix[1][0] * rotationAxisMatrix[0][0] + rotationMatrix[1][1] * rotationAxisMatrix[1][0] + rotationMatrix[1][2] * rotationAxisMatrix[2][0];
    rotationMatrixCombined[1][1] = rotationMatrix[1][0] * rotationAxisMatrix[0][1] + rotationMatrix[1][1] * rotationAxisMatrix[1][1] + rotationMatrix[1][2] * rotationAxisMatrix[2][1];
    rotationMatrixCombined[1][2] = rotationMatrix[1][0] * rotationAxisMatrix[0][2] + rotationMatrix[1][1] * rotationAxisMatrix[1][2] + rotationMatrix[1][2] * rotationAxisMatrix[2][2];
    rotationMatrixCombined[2][0] = rotationMatrix[2][0] * rotationAxisMatrix[0][0] + rotationMatrix[2][1] * rotationAxisMatrix[1][0] + rotationMatrix[2][2] * rotationAxisMatrix[2][0];
    rotationMatrixCombined[2][1] = rotationMatrix[2][0] * rotationAxisMatrix[0][1] + rotationMatrix[2][1] * rotationAxisMatrix[1][1] + rotationMatrix[2][2] * rotationAxisMatrix[2][1];
    rotationMatrixCombined[2][2] = rotationMatrix[2][0] * rotationAxisMatrix[0][2] + rotationMatrix[2][1] * rotationAxisMatrix[1][2] + rotationMatrix[2][2] * rotationAxisMatrix[2][2];

    float xRot, yRot, zRot;
    xRot = atan2(rotationMatrixCombined[2][1], rotationMatrixCombined[2][2]);
    zRot = atan2(rotationMatrixCombined[1][0], rotationMatrixCombined[0][0]);

    if (cosf(zRot) == 0.f) {
        yRot = atan2(-rotationMatrixCombined[2][0], rotationMatrixCombined[1][0] / sinf(zRot));
    }
    else {
        yRot = atan2(-rotationMatrixCombined[2][0], rotationMatrixCombined[0][0] / cosf(zRot));
    }

    //std::cout << "xRot = " << xRot * (180.f / 3.14159f) << "; yRot = " << yRot * (180.f / 3.14159f) << "; zRot = " << zRot * (180.f / 3.14159f) << std::endl;
    returnAngles = { xRot,yRot,zRot };


    int rowCount, columnCount;
    rowCount = 0;
    columnCount = 0;

    int times = 0;


    for (float i = 1.f - stepSize; i >= -1.f; i -= stepSize) {

        columnCount = 0;

        for (float j = -1.f; j <= 1.f; j += stepSize) {
            float distanceFromMiddle;
            distanceFromMiddle = sqrtf(powf(i, 2) + powf(j, 2));

            if (distanceFromMiddle <= 1.f) {
                float x, y, z;
                x = j;
                y = i;
                z = sqrtf(1.f - powf(x, 2) - powf(y, 2));

                float newX, newY, newZ;

                newX = x * rotationMatrixCombined[0][0] + y * rotationMatrixCombined[0][1] + z * rotationMatrixCombined[0][2];
                newY = x * rotationMatrixCombined[1][0] + y * rotationMatrixCombined[1][1] + z * rotationMatrixCombined[1][2];
                newZ = x * rotationMatrixCombined[2][0] + y * rotationMatrixCombined[2][1] + z * rotationMatrixCombined[2][2];



                if (abs(newX) < 0.01f) {
                    newX = 0.f;
                }

                if (abs(newY) < 0.01f) {
                    newY = 0.f;
                }

                if (abs(newZ) < 0.01f) {
                    newZ = 0.f;
                }


                float u, v;
                u = atan2f(newY, newX) / PI;
                v = (2.f * -asin(newZ)) / PI;

                sf::Vector2f normalizedCoords = { u, v };
                sf::Vector2f pixelCoords = convertToRegularCoords(normalizedCoords, (int)(theHeight - 1.f), (int)(theWidth - 1.f));
                sf::Color thePixel = originalImage.getPixel(pixelCoords.x, pixelCoords.y);
                sf::Vector2i targetPixelLocation = { rowCount, columnCount };
                editImage.setPixel(targetPixelLocation.y, targetPixelLocation.x, thePixel);


                if (pixelCoords.x >= theWidth || pixelCoords.y >= theHeight || pixelCoords.x < 0.f || pixelCoords.y < 0.f) {
                    std::cout << "normalizedCoords = (" << normalizedCoords.x << "," << normalizedCoords.y << "); ";
                    //std::cout << "actualPixel = (" << actualPixel.x << "," << actualPixel.y << ")" << std::endl;
                    std::cout << "(u,v) = (" << u << "," << v << ")" << std::endl;
                    std::cout << "x = " << x << ", y = " << y << ", z = " << z << std::endl;
                    return false;
                }

            }
            columnCount++;
        }
        rowCount++;
    }

    return true;

}