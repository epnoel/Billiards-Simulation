#pragma once

#include "Ball.h"
#include <vector>

class Animation {

public:
    Animation();

    void updateFNetAndTorqueXAxis();

    void updateFNetAndTorqueYAxis();

public:

    float frameRate;
    float gravity = 9.81f;
    float frictionCoefficient;
    float savedFrictionCoefficient;
    std::vector<float> fNet;
    std::vector<Ball> balls;


};