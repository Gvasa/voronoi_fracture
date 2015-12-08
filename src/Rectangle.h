#ifndef RECTANGLE_H
#define RECTANGLE_H

// Libs and headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <limits>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Classes
#include "Geometry.h"
#include "tools/shader.hpp"
#include "utils/Utils.h"

class Rectangle : public Geometry {

public:

    Rectangle(float, float, Vector3<float>);
    ~Rectangle();

    void initialize(Vector3<float>);
    void render(std::vector<Matrix4x4<float> >);

    void addVertex(Vector3<float> v) { mVerts.push_back(v); };
    void addNormal(Vector3<float> n) { mNormals.push_back(n); };
    void createVertices(float, float);

    void rotate(Vector3<float>, float);
    void translate(Vector3<float>);
    void scale(Vector3<float>);

    std::vector<Vector3<float> > getVertexList() { return mVerts; }
    glm::mat4 getTransMat() { return mTransMat; }

    unsigned int getType() { return RECTANGLE; }

private:

    Vector3<float> mPosition;

    std::vector<Vector3<float> > mVerts;
    std::vector<Vector3<float> > mNormals;

    // Shader data
    GLuint vertexArrayID;
    GLuint vertexBuffer;
    GLuint normalBuffer;
    GLuint shaderProgram;

    // Shader indices for uniforms, i.e. data that we upload to the GPU
    GLint MVPLoc;           // MVP matrix
    GLint MVLoc;            // MV matrix
    GLint MVLightLoc;       // MVLight matrix
    GLint NMLoc;            // NM matrix
    GLint lightPosLoc;      // Light position
    GLint colorLoc;         // Color
    GLint lightAmbLoc;      // Ambient light
    GLint lightDifLoc;      // Diffuse light
    GLint lightSpeLoc;      // Specular light
    GLint specularityLoc;   // Specular constant
    GLint shinynessLoc;     // How much specularity (magnitude)

    glm::mat4 mTransMat;

    struct Material : public Geometry::Material {   
    } mMaterial;
};

#endif // RECTANGLE_H