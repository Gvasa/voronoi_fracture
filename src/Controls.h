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

    Controls(float radius, float x = 512, float y = 384) 
            : _radius(radius), centerPosition(glm::vec2(x, y)) {}

    ~Controls();

    void dragStart(double x, double y);
    void dragUpdate(double x, double y);
    void dragEnd();
    bool dragged() const { return _dragged; }

    glm::quat& rotate(glm::quat &orientation, double x, double y);

    glm::vec2 direction(double x, double y);

    void center(float x, float y) {
        centerPosition.x = x;
        centerPosition.y = y;
    }
    

private:
    glm::vec2 centerPosition;
    glm::vec2 dragStartPosition;
    float _radius;
    bool _dragged;

    glm::vec3 map_to_sphere(const glm::vec2 &point);

};

#endif // CONTROLS_H