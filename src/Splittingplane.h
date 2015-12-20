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

    Splittingplane(std::pair<Vector3<float>, Vector3<float> > v, Vector3<float> n, Vector3<float> c)
        : mVoronoiPoints(v), mNormal(n), mCenterPoint(c) { }
    
    std::pair<Vector3<float>, Vector3<float> > getVoronoiPoints() { return mVoronoiPoints; }

    Vector3<float> getNormal() { return mNormal; }

    Vector3<float> getCenterPoint() { return mCenterPoint; }

private:

    std::pair<Vector3<float>, Vector3<float> > mVoronoiPoints;

    Vector3<float> mNormal;

    Vector3<float> mCenterPoint;

};

#endif // SPLITTINGPLANE_H