/******************************************************************************
 *
 * A class for handling of all geometries in a scene
 *
 *****************************************************************************/

#ifndef SCENE_H
#define SCENE_H
#define GLM_FORCE_RADIANS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Geometry.h"
#include "math/Matrix4x4.h"
#include "math/Vector3.h"




class Scene{

public:

    Scene();
    ~Scene();

    void initialize();
    void draw();

    void addGeometry(Geometry *);

private:

    Matrix4x4<float> toMatrix4x4(glm::mat4);

    std::vector<Geometry *>mGeometries;

};

#endif // SCENE_H