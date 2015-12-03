#include "SimpleMesh.h"

SimpleMesh::SimpleMesh() {

    std::cout << "\nCreating SimpleMesh...";

    mMaterial.color         = Vector4<float>(0.2f, 0.6f, 0.6f, 1.0f);
    mMaterial.ambient       = Vector4<float>(0.3f, 0.3f, 0.3f, 1.0f);
    mMaterial.diffuse       = Vector4<float>(0.8f, 0.8f, 0.8f, 1.0f);
    mMaterial.specular      = Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f);
    mMaterial.specularity   = 50.0f;
    mMaterial.shinyness     = 0.6f;

    std::cout << "Done!\n";
}


SimpleMesh::~SimpleMesh() {

    mVerts.clear();
    mVerts.shrink_to_fit();
}


void SimpleMesh::initialize(Vector3<float> lightPosition) {

}


void SimpleMesh::render(std::vector<Matrix4x4<float> > sceneMatrices) {

}


bool SimpleMesh::addFace(const std::vector<Vector3<float> >verts) {

    for(unsigned int i = 0; i < verts.size(); i++)
        mVerts.push_back(verts[i]);

}