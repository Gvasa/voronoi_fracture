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
    virtual void render(std::vector<Matrix4x4<float> >) = 0;


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
    virtual void addVoronoiPoint(Vector3<float>) {
          std::cout << "\nGeometry does not have addVoronoiPoint implemented!\n";
    }

protected:

    //! Compute and return the normal at face at faceIndex
    virtual Vector3<float> calculateFaceNormal(unsigned int faceIndex) const {
        std::cout << "\nGeometry does not have the function 'calculateFaceNormal' implemented\n";
    };

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

  struct Material {
        Vector4<float> color;
        Vector4<float> ambient;
        Vector4<float> diffuse;
        Vector4<float> specular;
        float specularity;
        float shinyness;
    };
};

#endif // GEOMETRY_H