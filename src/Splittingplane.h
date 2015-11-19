#ifndef SPLITTINGPLANE_H
#define SPLITTINGPLANE_H

#include <iomanip>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix4x4.h"
#include "tools/shader.hpp"
#include "utils/Utils.h"
#include "utils/Debugpoint.h"

class Splittingplane {

public:

    Splittingplane(
        std::vector<Vector3<float> >,
        std::vector<Vector3<float> >, 
        std::pair<Vector3<float>, Vector3<float> >,
        Vector3<float>, 
        Vector4<float> c = Vector4<float>(1.0f, 1.0f, 1.0f, 0.5f));
    
    ~Splittingplane();

    void initialize();

    void render(Matrix4x4<float>);

    void resolveIntersection(std::pair<Vector3<float>, Vector3<float> >);

    std::vector<Vector3<float> > getVertexList() { return mUniqueVerts; }

    void setColor(Vector4<float> c) { mColor = c; }

    void resetSplittingPlane();

    void buildRenderData();

    std::vector<Vector3<float> > getUniqueVerts() { return mUniqueVerts; }

private:

    std::vector<Vector3<float> > sortVertices(std::vector<Vector3<float> >);

    // Shader data
    GLuint vertexArrayID;
    GLuint vertexBuffer;
    GLuint shaderProgram;

    GLint MVPLoc;       // MVP Matrix
    GLint ColorLoc;     // Color Vector

    std::vector<Vector3<float> > mVerts;

    std::pair<Vector3<float>, Vector3<float> > mVoronoiPoints;

    std::vector<Vector3<float> > mUniqueVerts;

    std::vector<Vector3<float> > mBoundingValues;

    std::vector<std::pair<Vector3<float>, Vector3<float> > > mIntersectedPoints;

    Vector4<float> mColor;

    Vector3<float> mNormal;

    std::vector<Debugpoint *> mDebugPoints;
};

#endif // SPLITTINGPLANE_H