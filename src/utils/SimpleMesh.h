#ifndef SIMPLEMESH_H
#define SIMPLEMESH_H

// Libs and headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Classes
#include "../Geometry.h"
#include "../tools/shader.hpp"
#include "Utils.h"

class SimpleMesh : public Geometry {

public:

    SimpleMesh();

    ~SimpleMesh();

    void initialize(Vector3<float> l = Vector3<float>(0.0f, 0.0f, 0.0f));
    
    void render(std::vector<Matrix4x4<float> >);

    unsigned int getType() { return HALFEDGEMESH; }

    bool addFace(const std::vector<Vector3<float> >);

    unsigned int getNumberOfVerts() { return mVerts.size(); }
    
    std::vector<Vector3<float>  >getVertexList() { return mVerts; }

private:

    std::vector<Vector3<float> > mVerts;

    bool computeNormal(const std::vector<Vector3<float> >);

    struct Material : public Geometry::Material {   
    } mMaterial;

    // Shader data
    GLuint vertexArrayID;
    GLuint vertexBuffer;
    GLuint shaderProgram;

    // Shader indices for uniforms, i.e. data that we upload to the GPU
    GLint MVPLoc;           // MVP matrix
    GLint ColorLoc;         // Color

};

#endif // SIMPLEMESH_H