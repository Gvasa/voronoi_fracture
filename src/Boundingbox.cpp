#include "Boundingbox.h"

Boundingbox::Boundingbox(std::vector<Vector3<float> > uniqueVerts) {

    calculateBoundingbox(uniqueVerts);
}


Boundingbox::~Boundingbox() {
    mVerts.clear();
    //mVerts.shrink_to_fit();
}


void Boundingbox::initialize(Vector3<float> lightPosition) {

}


void Boundingbox::render(std::vector<Matrix4x4<float> > sceneMatrices) {

}


void Boundingbox::calculateBoundingbox(std::vector<Vector3<float> > uniqueVerts) {


    std::map<std::string, unsigned int> boundingValues;

    for(unsigned int i = 0; i < uniqueVerts.size(); i++) {

    }

}