#include "Controls.h"

void Controls::dragStart(double x, double y) {

    mDragged = true;
    dragUpdate(x, y);
}

// update the mouse current position
void Controls::dragUpdate(double x, double y) {
    
    if(mDragged) {
        mDragStartPosition.x = x - mCenterPosition.x;
        mDragStartPosition.y = y - mCenterPosition.y;
    }
}

// notify that the mouse is not moving
void Controls::dragEnd() {
    
    if(mDragged) {
        dragUpdate(0.0f, 0.0f);
        mDragged = false;
    }
}

//returns the direction of the movement done
glm::vec2 Controls::direction(double x, double y) {
    
    glm::vec2 dragEndPosition(x - mCenterPosition.x, y - mCenterPosition.y);
    glm::vec2 v(dragEndPosition.x - mDragStartPosition.x, dragEndPosition.y - mDragStartPosition.y);
    v.y = -v.y;

    return glm::normalize(v);
}

glm::quat& Controls::rotate(glm::quat &orientation, double x, double y) {

    if(!mDragged)
        return orientation;

    glm::vec3 v0 = map_to_sphere(mDragStartPosition);
    glm::vec3 v1 = map_to_sphere(glm::vec2(x - mCenterPosition.x, y - mCenterPosition.y));
    glm::vec3 v2 = glm::cross(v0, v1); // get which axis we should rotate around.

    float d = glm::dot(v0, v1);
    float s = sqrtf((1.0f + d) * 2.0f);
    glm::quat q(0.5f * s, v2 / s);

    orientation = q * orientation; // apply rotation
    orientation /= glm::length(orientation); // normalize
    
    return orientation;
}

//map our 2d position on the screen to the 3d pos of sphere in center.
glm::vec3 Controls::map_to_sphere(const glm::vec2 &point) {

    glm::vec2 p(point.x, point.y);

    p.y = -p.y;

    float safeRadius = mRadius - 1.0f; // safe radius is       

    if(glm::length(p) > safeRadius) {
        float theta = atan2(p.y, p.x);
        p.x = safeRadius * cos(theta);
        p.y = safeRadius * sin(theta);
    }

    float lengthSquared = pow(p.x, 2) + pow(p.y, 2);
    float z = sqrt(pow(mRadius, 2) - lengthSquared);
    glm::vec3 q(p.x, p.y, z);

    return glm::normalize(q / mRadius);
} 