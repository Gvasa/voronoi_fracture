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
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>

//#include "Geometry.h"
#include "ClippingMesh.h"
#include "HalfEdgeMesh.h"
#include "utils/SimpleMesh.h"
#include "math/Matrix4x4.h"
#include "math/Vector3.h"
#include "Controls.h"
#include "utils/Utils.h"
#include "Physics.h"

class HalfEdgeMesh;

class Scene{

public:

    Scene();
    
    ~Scene();

    void initialize();
    
    void render();

    void addGeometry(Geometry *, unsigned int);

    void updateCameraPosition(double, double);

    void updateCameraZoom(double, double);
    
    void mouseButtonClick(double x, double y) { control->dragStart(x, y); }
    
    void mouseButtonRelease() { control->dragEnd(); }
    
    void resetCamera();

    void stepSimulation();

    void splitMesh(HalfEdgeMesh *);

    void setInitialVelocity(unsigned int i, Vector3<float> v) { physicsWorld->setInitialVelocity(i, v); }

    void setPreComputedVoronoiPoints(std::map<std::string, std::vector<Vector3<float> > > v) { mVoronoiPoints = v; }

    std::vector<Vector3<float> > getPreComputedVoronoiPoints(std::string s) { return mVoronoiPoints[s]; }

    void updatePreComputedVoronoiPattern(std::string, glm::mat4);

private:

    Matrix4x4<float> toMatrix4x4Row(glm::mat4);

    Matrix4x4<float> toMatrix4x4Column(glm::mat4);
    
    Matrix4x4<float> toMatrix4x4(glm::mat3);

    glm::mat4 toGlmMat4(float []);

    std::vector<Geometry *>mGeometries;
    
    std::vector<Matrix4x4<float> >mSceneMatrices;

    Physics* physicsWorld;

    std::vector<std::pair<Geometry*, btRigidBody*> > mObjects;

    std::map<std::string, std::vector<Vector3<float> > > mVoronoiPoints;

    struct LightSource {
        Vector4<float> color;
        Vector3<float> position;
    } mPointLight;

    Controls *control;
};

#endif // SCENE_H