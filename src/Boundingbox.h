#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <map>
#include <iomanip>
#include <chrono>
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
    void setWireFrame(bool w) { mWireframe = w; };

    std::map<std::string, unsigned int> getBoundingValuesIndex() { return mBoundingValuesIndex; }
    std::vector<Vector3<float> > getBoundingValues() { return mBoundingValues; }

private:

    // Shader data
    GLuint vertexArrayID;
    GLuint vertexBuffer;
    GLuint shaderProgram;

    GLint MVPLoc; // MVP Matrix


    void calculateBoundingbox(std::vector<Vector3<float> >);
    void calculateConvexHull(std::vector<Vector3<float> >);

    std::vector<Vector3<float> > mVerts;
    std::vector<Vector3<float> > mBoundingValues;
    std::map<std::string, unsigned int> mBoundingValuesIndex;

    bool mWireframe = false;


};

#endif // BOUNDINGBOX_H