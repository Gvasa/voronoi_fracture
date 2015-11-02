/******************************************************************************
 *
 * An abstraction class for handling of different types of geometries
 * 
 *****************************************************************************/

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix4x4.h"

class Geometry {

public: 

    virtual ~Geometry() {};

    // A Geometry must have the following:
    virtual void initialize() = 0;
    virtual void draw(Matrix4x4<float>) = 0;
    virtual bool addFace() = 0;

    // A Geometry might have the following:
    // Add functions
};

#endif // GEOMETRY_H