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

    void resolveIntersection(std::pair<Vector3<float>, Vector3<float> >, Vector3<float>);

    void setColor(Vector4<float> c) { mColor = c; }

    void resetSplittingPlane();

    void buildRenderData();

    std::vector<Vector3<float> > getUniqueVerts() { return mUniqueVerts; }

    std::pair<Vector3<float>, Vector3<float> > getVoronoiPoints() { return mVoronoiPoints; }

    Vector3<float> getNormal() { return mNormal; }

    std::vector<Vector3<float> > getVertexList() { return mUniqueVerts; }

    Vector3<float> getVertex(unsigned int i) { debug
                                                return mUniqueVerts[i]; }

    unsigned int getNumVerts() { return mUniqueVerts.size(); }

    void updateSplittingPlane(glm::mat4);

private:

    // Member functions

    std::vector<Vector3<float> > sortVertices(std::vector<Vector3<float> >);

    // Shader data
    GLuint vertexArrayID;
    
    GLuint vertexBuffer;
    
    GLuint shaderProgram;

    GLint MVPLoc;       // MVP Matrix
    
    GLint ColorLoc;     // Color Vector

    // Instance variables

    std::vector<Vector3<float> > mVerts;

    std::pair<Vector3<float>, Vector3<float> > mVoronoiPoints;

    std::vector<Vector3<float> > mUniqueVerts;

    std::vector<Vector3<float> > mBoundingValues;

    std::vector<std::pair<Vector3<float>, Vector3<float> > > mIntersectedPoints;

    Vector4<float> mColor;

    Vector3<float> mNormal;

    std::vector<Debugpoint *> mDebugPoints;

    std::vector<std::pair<std::pair<unsigned int, unsigned int>, std::pair<Vector3<float>, Vector3<float> > > > mPlaneIntersections;

};

#endif // SPLITTINGPLANE_H