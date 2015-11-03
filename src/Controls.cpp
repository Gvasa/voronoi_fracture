#include "Controls.h"


Controls::~Controls() {

 
}

void Controls::dragStart(double x, double y) {
    _dragged = true;
    dragUpdate(x, y);
}

//update the mouse current position
void Controls::dragUpdate(double x, double y) {
    if(_dragged) {
        dragStartPosition.x = x - centerPosition.x;
        dragStartPosition.y = y - centerPosition.y;
    }
}

//notify that the mouse is not moving
void Controls::dragEnd() {
    if(_dragged) {
        dragUpdate(0.0f, 0.0f);
        _dragged = false;
    }
}

//returns the direction of the movement done
glm::vec2 Controls::direction(double x, double y) {
    glm::vec2 dragEndPosition(x - centerPosition.x, y - centerPosition.y);
    glm::vec2 v(dragEndPosition.x - dragStartPosition.x, dragEndPosition.y - dragStartPosition.y);
    v.y = -v.y;
    return glm::normalize(v);
}

glm::quat& Controls::rotate(glm::quat &orientation, double x, double y) {

    if(!_dragged)
        return orientation;

    glm::vec3 v0 = map_to_sphere(dragStartPosition);
    glm::vec3 v1 = map_to_sphere(glm::vec2(x - centerPosition.x, y - centerPosition.y));
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

    float safeRadius = _radius - 1.0f;       // safe radius is       

    if(glm::length(p) > safeRadius) {
        float theta = atan2(p.y, p.x);
        p.x = safeRadius * cos(theta);
        p.y = safeRadius * sin(theta);
    }

    float lengthSquared = pow(p.x, 2) + pow(p.y, 2);
    float z = sqrt(pow(_radius, 2) - lengthSquared);
    glm::vec3 q(p.x, p.y, z);

    return glm::normalize(q / _radius);
} 