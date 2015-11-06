#ifndef COMPOUND_H
#define COMPOUND_H

#define XMAX 0
#define XMIN 1
#define YMAX 2
#define YMIN 3
#define ZMAX 4
#define ZMIN 5


#include <map>
#include <iomanip>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix4x4.h"
#include "tools/shader.hpp"
#include "Boundingbox.h"
#include "utils/Utils.h"

class Compound {

public:

    Compound(Boundingbox*, std::vector<Vector3<float> >);
    ~Compound();

    void initialize();
    void render(Matrix4x4<float>);
    void setWireFrame(bool w) { mWireframe = w; };

private:

    // Shader data
    GLuint vertexArrayID;
    GLuint vertexBuffer;
    GLuint shaderProgram;

    GLint MVPLoc; // MVP Matrix


    void calculateConvexHull(Boundingbox *, std::vector<Vector3<float> >);

    std::vector<Vector3<float> > mVerts;

    bool mWireframe = false;
};

#endif // COMPOUND_H