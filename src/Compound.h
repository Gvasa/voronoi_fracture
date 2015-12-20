#ifndef COMPOUND_H
#define COMPOUND_H

#include <map>
#include <iomanip>
#include <algorithm>
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

    Splittingplane* getSplittingPlane(unsigned int i) { return mSplittingPlanes[i]; }

    unsigned int getNumberOfSplittingPlanes() { return mSplittingPlanes.size(); }


private:

    // Member functions

    void calculateVoronoiPattern(Boundingbox *, std::vector<Vector3<float> >);
    
    void calculateSplittingPlane(Boundingbox *, std::pair<Vector3<float>, Vector3<float> >);

    // Instance variables

    std::vector<Splittingplane *> mSplittingPlanes;

    std::vector<Vector3<float> > mVoronoiPoints;
};

#endif // COMPOUND_H