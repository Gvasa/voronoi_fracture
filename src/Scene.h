/******************************************************************************
 *
 * A class for handling of all geometries in a scene
 *
 *****************************************************************************/

#ifndef SCENE_H
#define SCENE_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Geometry.h"

class Scene{

public:

    Scene();
    ~Scene();

    void init();
    void draw();

    void addGeometry(Geometry *);

private:
    std::vector<Geometry *>mGeometries;

};

#endif // SCENE_H