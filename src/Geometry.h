/******************************************************************************
 *
 * An abstraction class for handling of different types of geometries
 * 
 *****************************************************************************/

#ifndef GEOMETRY_H
#define GEOMETRY_H

#define I_MVP 0
#define I_MV 1
#define I_MV_LIGHT 2
#define I_NM 3

#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix4x4.h"

class Geometry {

public: 

    virtual ~Geometry() {};

    /* 
     * A Geometry must have the following: 
     */
    virtual void initialize(Vector3<float>) = 0;
    virtual void draw(std::vector<Matrix4x4<float> >) = 0;

    /* 
     * A Geometry might have the following: 
     */
    virtual bool addFace(std::vector<Vector3<float> >) {};
    
    // Matrix operations
    virtual void rotate(Vector3<float>, float) {
        std::cout << "\nGeometry does not have rotation implemented!\n";
    };
    virtual void translate(Vector3<float>) {
        std::cout << "\nGeometry does not have translation implemented!\n";
    };
    virtual void scale(Vector3<float>) {
        std::cout << "\nGeometry does not have scaling implemented!\n";
    };
};

#endif // GEOMETRY_H