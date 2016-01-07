
#ifndef UTILS_H
#define UTILS_H

#define GLM_FORCE_RADIANS

#define EPSILON 0.01
#define EPSILON_2 0.001

#define XMAX 0
#define XMIN 1
#define YMAX 2
#define YMIN 3
#define ZMAX 4
#define ZMIN 5
#define XPROJECTION 0
#define YPROJECTION 1
#define ZPROJECTION 2
#define MAX_VORONOI_POINTS 3
#define RECTANGLE 0
#define HALFEDGEMESH 1
#define DEBUGPOINT 2
#define SIMPLEMESH 3
#define STATIC 0
#define DYNAMIC 1

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <map>
#include <vector>

#include "../math/Vector4.h"
#include "../math/Vector3.h"
#include "../math/Matrix4x4.h"

#define debug std::cout << "vid: " << __LINE__ << " " << __FUNCTION__ << std::endl << std::endl;

static std::vector<Vector3<float> > getVoronoiPattern(std::string obj);
static void updateVoronoiPatterns(std::string, Matrix4x4<float>);
static bool createPreDefinedVoronoiPoints();

const int WIDTH = 1024;
const int HEIGHT = 768;

// Set constants for flaging of edges that are not valid half-edges
const static unsigned int BORDER = (std::numeric_limits<unsigned int>::max)();
const static unsigned int UNINITIALIZED = (std::numeric_limits<unsigned int>::max)()-1;

class Utils {

public:

    static void printQuat(glm::quat q) {
        std::cout << glm::to_string(glm::eulerAngles(q)) << std::endl;
    }

private:

};

static Vector4<float> mColorScale[12] = {
        Vector4<float>(166.0f/255.0f, 206.0f/255.0f, 227.0f/255.0f, 0.5f),
        Vector4<float>(31.0f /255.0f, 120.0f/255.0f, 180.0f/255.0f, 0.5f),
        Vector4<float>(178.0f/255.0f, 223.0f/255.0f, 138.0f/255.0f, 0.5f),
        Vector4<float>(51.0f /255.0f, 160.0f/255.0f, 44.0f /255.0f, 0.5f),
        Vector4<float>(251.0f/255.0f, 154.0f/255.0f, 153.0f/255.0f, 0.5f),
        Vector4<float>(227.0f/255.0f, 26.0f /255.0f, 28.0f /255.0f, 0.5f),
        Vector4<float>(253.0f/255.0f, 191.0f/255.0f, 111.0f/255.0f, 0.5f),
        Vector4<float>(255.0f/255.0f, 127.0f/255.0f, 0.0f  /255.0f, 0.5f),
        Vector4<float>(202.0f/255.0f, 178.0f/255.0f, 214.0f/255.0f, 0.5f),
        Vector4<float>(106.0f/255.0f, 61.0f /255.0f, 154.0f/255.0f, 0.5f),
        Vector4<float>(255.0f/255.0f, 255.0f/255.0f, 153.0f/255.0f, 0.5f),
        Vector4<float>(177.0f/255.0f, 89.0f /255.0f, 40.0f /255.0f, 0.5f)
    };

static unsigned int mColorScaleCounter = 0;

static Vector4<float> getColor(unsigned int i) { return mColorScale[i]; }

static std::map<std::string, std::vector<Vector3<float> > > sVoronoiPoints;

static bool createPreDefinedVoronoiPoints() {

    std::vector<Vector3<float> > VP;

    VP.push_back(Vector3<float>(-0.35f, 0.15f, 0.0f));
    VP.push_back(Vector3<float>(-0.8f, 0.65f, 0.05f));
    VP.push_back(Vector3<float>(-0.5f, 1.05f, -0.2f));
    VP.push_back(Vector3<float>(-0.95f, 1.0f, -0.5f));
    sVoronoiPoints["bunnySmall_reduced"] = VP;

    VP.clear();
    VP.shrink_to_fit();

    VP.push_back(Vector3<float>(-0.6f, 0.41f, 0.1f));
    VP.push_back(Vector3<float>(0.15f, 0.5f, 0.55f));
    VP.push_back(Vector3<float>(0.7f, 0.45f, -0.3f));
    VP.push_back(Vector3<float>(-0.35f, -0.65f, 0.35f));
    sVoronoiPoints["icosphere"] = VP;

    VP.clear();
    VP.shrink_to_fit();

    VP.push_back(Vector3<float>(-0.7f, 0.15f, 0.0f));
    VP.push_back(Vector3<float>(-0.25f, 0.45f, 0.0f));
    VP.push_back(Vector3<float>(0.15f, 0.15f, 0.0f));
    VP.push_back(Vector3<float>(0.6f, 0.35f, 0.0f));
    VP.push_back(Vector3<float>(0.7f, 0.4f, 0.0f));
    sVoronoiPoints["cow_2"] = VP;

    VP.clear();
    VP.shrink_to_fit();

    VP.push_back(Vector3<float>(0.0f, -0.05f, 0.0f));
    VP.push_back(Vector3<float>(-0.1f, 0.2f, -0.05f));
    VP.push_back(Vector3<float>(0.1f, 0.2f, 0.05f));
    VP.push_back(Vector3<float>(0.0f, 0.35f, 0.0f));
    VP.push_back(Vector3<float>(0.0f, 0.6f, 0.0f));
    sVoronoiPoints["pillar"] = VP;

    VP.clear();
    VP.shrink_to_fit();
}

#endif // UTILS_H