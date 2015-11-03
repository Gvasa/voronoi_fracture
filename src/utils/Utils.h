
#ifndef UTILS_H
#define UTILS_H

#define GLM_FORCE_RADIANS

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <stdio.h>
#include <stdlib.h>

#define debug std::cout << "vid: " << __LINE__ << " " << __FUNCTION__ << std::endl << std::endl;

const int WIDTH = 1024;
const int HEIGHT = 768;

class Utils {

public:

static void printQuat(glm::quat q) {
    std::cout << glm::to_string(glm::eulerAngles(q)) << std::endl;
}


private:

};

#endif // UTILS_H