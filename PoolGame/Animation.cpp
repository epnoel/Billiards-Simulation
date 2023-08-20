#include "Animation.h"

Animation::Animation() {
    frictionCoefficient = 0.8f;
    savedFrictionCoefficient = frictionCoefficient;

    updateFNetAndTorqueXAxis();
    updateFNetAndTorqueYAxis();

}

void Animation::updateFNetAndTorqueXAxis() {
    for (int i = 0; i < balls.size(); i++) {

        if (balls[i].velocity.x == 0.f) {
            balls[i].frictionCoefficient.x = 0.f;
        }
        else if (balls[i].velocity.x > 0.f) {

            balls[i].frictionCoefficient.x = 0.f - abs(savedFrictionCoefficient);

            balls[i].fNet = balls[i].mass * gravity * frictionCoefficient;
            //balls[i].fNet = (balls[i].mass * balls[i].velocity.x) / 0.02f - balls[i].mass * gravity * frictionCoefficient;
            //balls[i].fNet = balls[i].mass * balls[i].velocity.x / 0.02f;
        }
        else {

            balls[i].frictionCoefficient.x = abs(savedFrictionCoefficient);

            balls[i].fNet = balls[i].mass * gravity * frictionCoefficient;
            //balls[i].fNet = balls[i].mass * balls[i].velocity.x / 0.02f;
        }

        balls[i].torque = -(balls[i].radius * balls[i].fNet);
    }
}

void Animation::updateFNetAndTorqueYAxis() {
    for (int i = 0; i < balls.size(); i++) {

        if (balls[i].velocity.y == 0.f) {
            balls[i].frictionCoefficient.y = 0.f;
        }
        else if (balls[i].velocity.y > 0) {

            balls[i].frictionCoefficient.y = 0.f - abs(savedFrictionCoefficient);

            balls[i].fNet = balls[i].mass * gravity * frictionCoefficient;
            //balls[i].fNet = (balls[i].mass * balls[i].velocity.x) / 0.02f - balls[i].mass * gravity * frictionCoefficient;
            //balls[i].fNet = balls[i].mass * balls[i].velocity.x / 0.02f;
        }
        else {

            balls[i].frictionCoefficient.y = abs(savedFrictionCoefficient);

            balls[i].fNet = balls[i].mass * gravity * frictionCoefficient;
            //balls[i].fNet = balls[i].mass * balls[i].velocity.x / 0.02f;
        }

        balls[i].torque = -(balls[i].radius * balls[i].fNet);
    }
}