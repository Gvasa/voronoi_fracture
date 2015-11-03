
#ifndef UTILS_H
#define UTILS_H
#define debug std::cout << "vid: " << __LINE__ << " " << __FUNCTION__ << std::endl << std::endl;

#include <iostream>
#include <glm/gtx/string_cast.hpp>

class Utils {

public:

static void printQuat(glm::quat q) {
    std::cout << glm::to_string(glm::eulerAngles(q)) << std::endl;
}


private:

};

#endif // UTILS_H