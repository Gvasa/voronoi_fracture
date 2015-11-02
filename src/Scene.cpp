#include "Scene.h"

Scene::Scene() {

}


Scene::~Scene() {
    
    std::cout << "Cleaning up...\t";

    for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it)
        (*it)->~Geometry();

    std::cout << "Clean-up done" << std::endl;
}

// Set init stuff that applies to all geometries here
void Scene::initialize() {

    // Background color
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

    for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it)
        (*it)->initialize();
}

// Draw all geometries
void Scene::draw() {

    const glm::mat4 projectionMatrix = glm::perspective(
        45.0f,          // Field of view
        4.0f / 3.0f,    // Aspect ratio
        0.1f,           // Near clipping plane
        100.0f);        // Far clipping plane

    const glm::mat4 viewMatrix = glm::lookAt(
        glm::vec3(2, 2, 4),     // Camera / eye position
        glm::vec3(0, 0, 0),     // Target, what do we look at
        glm::vec3(0, 1, 0));    // Up-vector

    // The scene modelmatrix is nothing atm, the geometries will have their own model transforms
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // Construct MVP matrix
    Matrix4x4<float> MVP = toMatrix4x4(projectionMatrix * viewMatrix * modelMatrix);

    // Draw Geometries in scene
    for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it)
        (*it)->draw(MVP);
}


void Scene::addGeometry(Geometry *G) {
    mGeometries.push_back(G);
}


Matrix4x4<float> Scene::toMatrix4x4(glm::mat4 m) {
    float M[4][4] = {
        {m[0][0], m[0][1], m[0][2], m[0][3]},
        {m[1][0], m[1][1], m[1][2], m[1][3]},
        {m[2][0], m[2][1], m[2][2], m[2][3]},
        {m[3][0], m[3][1], m[3][2], m[3][3]}
    };
    return Matrix4x4<float>(M);
}