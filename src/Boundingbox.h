#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <map>
#include <iomanip>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix4x4.h"
#include "tools/shader.hpp"

class Boundingbox {

public:

    Boundingbox(std::vector<Vector3<float> >);
    ~Boundingbox();

    void initialize();
    void render(Matrix4x4<float>);

private:

    // Shader data
    GLuint vertexArrayID;
    GLuint vertexBuffer;
    GLuint shaderProgram;

    GLint MVPLoc; // MVP Matrix


    void calculateBoundingbox(std::vector<Vector3<float> >);

    std::vector<Vector3<float> > mVerts;

    bool drawWireframe = true;
};

#endif // BOUNDINGBOX_H