#ifndef COMPOUND_H
#define COMPOUND_H

#include <map>
#include <iomanip>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix4x4.h"
#include "tools/shader.hpp"
#include "Boundingbox.h"
#include "Splittingplane.h"
#include "utils/Utils.h"
#include "utils/Debugpoint.h"

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
    GLuint colorBuffer;
    GLuint shaderProgram;

    GLint MVPLoc; // MVP Matrix
    GLint ColorLoc;
    Vector4<float> mColor;

    void calculateVoronoiPattern(Boundingbox *, std::vector<Vector3<float> >);
    
    void calculateSplittingPlane(Boundingbox *, std::pair<Vector3<float>, Vector3<float> >, unsigned int);

    bool calculatePlaneIntersection( std::vector<Vector3<float> >, std::vector<Vector3<float> >, std::pair<Vector3<float>, Vector3<float> > &);
    
    bool calculateLineIntersectionPoint(std::pair<Vector3<float>, Vector3<float> >, std::pair<Vector3<float>, Vector3<float> >, Vector3<float> &);
    
    std::vector<Vector3<float> > sortVertices(std::vector<Vector3<float> >, Vector3<float>);
    
    bool compareAngle(std::pair<float, Vector3<float> > p1, std::pair<float, Vector3<float> > p2) { return p1.first < p2.first; }

    std::vector<Vector3<float> > mVerts;
    std::vector<Vector3<float> > mColors;
    std::vector<Splittingplane *> mSplittingPlanes;
    std::vector<Debugpoint *> mDebugpoints;
    std::vector<Vector3<float> > mBoundingValues;

    bool mWireframe = false;

    std::vector<Vector3<float> > mColorScale;
};

#endif // COMPOUND_H