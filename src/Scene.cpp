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
void Scene::init() {

    // Background color
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

    for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it)
        (*it)->init();
}

// Draw all geometries
void Scene::draw() {
    for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it)
        (*it)->draw();
}


void Scene::addGeometry(Geometry *G) {
    mGeometries.push_back(G);
}