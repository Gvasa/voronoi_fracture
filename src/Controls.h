/******************************************************************************
 *
 * A class for handling of all controls 
 *
 *****************************************************************************/

#ifndef CONTROLS_H
#define CONTROLS_H

#define GLM_FORCE_RADIANS

#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

#include "utils/Utils.h"

class Controls{

public:

    Controls(float radius, float x = WIDTH, float y = HEIGHT) 
            : mRadius(radius), mCenterPosition(glm::vec2(x/2.0, y/2.0)) {}

    ~Controls();

    void dragStart(double x, double y);
    void dragUpdate(double x, double y);
    void dragEnd();
    bool dragged() const { return mDragged; }

    glm::quat& rotate(glm::quat &orientation, double x, double y);

    glm::vec2 direction(double x, double y);

    void center(float x, float y) {
        mCenterPosition.x = x;
        mCenterPosition.y = y;
    }
    

private:
    glm::vec2 mCenterPosition;
    glm::vec2 mDragStartPosition;
    float mRadius;
    bool mDragged;

    glm::vec3 map_to_sphere(const glm::vec2 &point);

};

#endif // CONTROLS_H