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
    virtual bool addFace(std::vector<Vector3 <float> >) = 0;

protected:

    //! Compute and return the normal at face at faceIndex
    virtual Vector3<float> calculateFaceNormal(unsigned int faceIndex) const = 0;

public:

    //! Minimal requirements for all meshes, inherited
    struct Face{
        Face(const Vector3<float> & n = Vector3<float>(0,0,0),
            const Vector3<float> & c = Vector3<float>(0.5,0.1,0.7),
            float u = 0) 
            : normal(n), 
            color(c), 
            curvature(u) {}
        Vector3<float> normal;
        Vector3<float> color;
        float curvature;
    };
    //! Minimal requirements for all meshes, inherited
    struct Vertex{
        Vertex(const Vector3<float> & p = Vector3<float>(0,0,0),
            const Vector3<float> & n = Vector3<float>(0,0,0),
            const Vector3<float> & c = Vector3<float>(0.5,0.1,0.7),
            float u = 0)
            : pos(p),
            normal(n),
            color(c),
            curvature(u) {}
        Vector3<float> pos;
        Vector3<float> normal;
        Vector3<float> color;
        float curvature;
  };



    // A Geometry might have the following:
    // Add functions
};

#endif // GEOMETRY_H