
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

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../math/Vector4.h"

#define debug std::cout << "vid: " << __LINE__ << " " << __FUNCTION__ << std::endl << std::endl;

const int WIDTH = 1024;
const int HEIGHT = 768;

//class LoadObj;


class Utils {

public:

    static void printQuat(glm::quat q) {
        std::cout << glm::to_string(glm::eulerAngles(q)) << std::endl;
    }

private:

};

static Vector4<float> mColorScale[12] = {
        Vector4<float>(166.0f/255.0f,206.0f/255.0f,227.0f/255.0f, 0.5f),
        Vector4<float>(31.0f/255.0f,120.0f/255.0f,180.0f/255.0f, 0.5f),
        Vector4<float>(178.0f/255.0f,223.0f/255.0f,138.0f/255.0f, 0.5f),
        Vector4<float>(51.0f/255.0f,160.0f/255.0f,44.0f/255.0f, 0.5f),
        Vector4<float>(251.0f/255.0f,154.0f/255.0f,153.0f/255.0f, 0.5f),
        Vector4<float>(227.0f/255.0f,26.0f/255.0f,28.0f/255.0f, 0.5f),
        Vector4<float>(253.0f/255.0f,191.0f/255.0f,111.0f/255.0f, 0.5f),
        Vector4<float>(255.0f/255.0f,127.0f/255.0f,0.0f/255.0f, 0.5f),
        Vector4<float>(202.0f/255.0f,178.0f/255.0f,214.0f/255.0f, 0.5f),
        Vector4<float>(106.0f/255.0f,61.0f/255.0f,154.0f/255.0f, 0.5f),
        Vector4<float>(255.0f/255.0f,255.0f/255.0f,153.0f/255.0f, 0.5f),
        Vector4<float>(177.0f/255.0f,89.0f/255.0f,40.0f/255.0f, 0.5f)
    };

static Vector4<float> getColor(unsigned int i) { return mColorScale[i]; }

#endif // UTILS_H