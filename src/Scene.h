/******************************************************************************
 *
 * A class for handling of all geometries in a scene
 *
 *****************************************************************************/

#ifndef SCENE_H
#define SCENE_H
#define GLM_FORCE_RADIANS

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
#include "glm/gtc/matrix_inverse.hpp"
#include <glm/gtc/quaternion.hpp>

#include "Geometry.h"
#include "math/Matrix4x4.h"
#include "math/Vector3.h"
#include "Controls.h"
#include "utils/Utils.h"



class Scene{

public:

    Scene();
    ~Scene();

    void initialize();
    void render();

    void addGeometry(Geometry *);
    void updateCameraPosition(double x, double y);
    void mouseButtonClick(double x, double y) { control.dragStart(x, y); }
    void mouseButtonRelease() { control.dragEnd(); }

private:

    Matrix4x4<float> toMatrix4x4(glm::mat4);
    Matrix4x4<float> toMatrix4x4(glm::mat3);

    std::vector<Geometry *>mGeometries;
    std::vector<Matrix4x4<float> >mSceneMatrices;

    struct LightSource {
        Vector4<float> color;
        Vector3<float> position;
    } mPointLight;
    Controls control = Controls(300.f);
};

#endif // SCENE_H