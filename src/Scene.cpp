#include "Scene.h"

Scene::Scene() {

    control = new Controls(300.f);
    physicsWorld = new Physics();
}


Scene::~Scene() {
    
    std::cout << "Cleaning up...\t";

    for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it)
        (*it)->~Geometry();

    delete control;

    std::cout << "Clean-up done" << std::endl;
}

// Set init stuff that applies to all geometries here
void Scene::initialize() {

    // Init the lightsource parameters
    mPointLight.position = Vector3<float>(0.0f, 0.0f, 3.0f);
    mPointLight.color = Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f);

    // Background color
    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

    mSceneMatrices.push_back(Matrix4x4<float>());
    mSceneMatrices.push_back(Matrix4x4<float>());
    mSceneMatrices.push_back(Matrix4x4<float>());
    mSceneMatrices.push_back(Matrix4x4<float>());

    for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it)
        (*it)->initialize(mPointLight.position);

    // Control the volume
    float volume = 0;

    for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it){
        if((*it)->getType() == HALFEDGEMESH)
            volume += (*it)->volume();
    }
    std::cout << "\nMesh volume: " << volume << std::endl;
}

// render all geometries
void Scene::render() {

    glEnable( GL_CULL_FACE );
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mCamera.projectionMatrix = glm::perspective(
        mCamera.fov,          // field of view, 45.0
        mCamera.aspectRatio,  // 4/3 atm
        0.1f,                // Near clipping plane
        100.0f);             // far clipping plane

    mCamera.viewMatrix = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f+mCamera.zoom),            // Camera / eye position
        glm::vec3(0.0f, 0.0f, 0.0f),            // Target, what to look at
        glm::vec3(0.0f, 1.0f, 0.0f)) *          // Up-vector                            
        glm::mat4_cast(mCamera.orientation);     // multiplies the veiw matrix with current rotation

    glm::mat4 modelMatrix;
    // render Geometries in scene
    for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it) {

        // The scene modelmatrix is nothing atm, the geometries will have their own model transforms
        modelMatrix = (*it)->getTransMat();

        // Construct MVP matrix
        mSceneMatrices[I_MVP] = toMatrix4x4Row(mCamera.projectionMatrix * mCamera.viewMatrix * modelMatrix);

        // Modelview Matrix, apply camera transforms here as well
        mSceneMatrices[I_MV] = toMatrix4x4Row(mCamera.viewMatrix * modelMatrix);

        // Modelview Matrix for our light
        mSceneMatrices[I_MV_LIGHT] = toMatrix4x4Row(mCamera.viewMatrix * modelMatrix);

        // Normal Matrix, used for normals in our shading
        mSceneMatrices[I_NM] = toMatrix4x4Row(glm::inverseTranspose(glm::mat4(mCamera.viewMatrix * modelMatrix)));

        (*it)->render(mSceneMatrices);
    }

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}


void Scene::addGeometry(Geometry *G, unsigned int type) {

    //updateVoronoiPatterns("icosphere", Matrix4x4<float>());
    mGeometries.push_back(G);
    G->calculateCenterOfMass();
    physicsWorld->addGeometry(G->getUniqueVertexList(), G->getWorldCenterOfMass() , type, G->volume());

}

Matrix4x4<float> Scene::toMatrix4x4Row(glm::mat4 m) {
    float M[4][4] = {
        {m[0][0], m[0][1], m[0][2], m[0][3]},
        {m[1][0], m[1][1], m[1][2], m[1][3]},
        {m[2][0], m[2][1], m[2][2], m[2][3]},
        {m[3][0], m[3][1], m[3][2], m[3][3]}
    };
    return Matrix4x4<float>(M);
}


Matrix4x4<float> Scene::toMatrix4x4Column(glm::mat4 m) {
    float M[4][4] = {
        {m[0][0], m[1][0], m[2][0], m[3][0]},
        {m[0][1], m[1][1], m[2][1], m[3][1]},
        {m[0][2], m[1][2], m[2][2], m[3][2]},
        {m[0][3], m[2][3], m[2][3], m[3][3]}
    };
    return Matrix4x4<float>(M);
}

Matrix4x4<float> Scene::toMatrix4x4(glm::mat3 m) {
    float M[4][4] = {
        {m[0][0], m[0][1], m[0][2], 0.0f},
        {m[1][0], m[1][1], m[1][2], 0.0f},
        {m[2][0], m[2][1], m[2][2], 0.0f},
        {0.0f   , 0.0f   , 0.0f   , 1.0f}
    };
    return Matrix4x4<float>(M);
}

glm::mat4 Scene::toGlmMat4(float m[]) {

    glm::mat4 M;

    M[0][0] = m[0];
    M[0][1] = m[1];
    M[0][2] = m[2];
    M[0][3] = m[3];
    M[1][0] = m[4];
    M[1][1] = m[5];
    M[1][2] = m[6];
    M[1][3] = m[7];
    M[2][0] = m[8];
    M[2][1] = m[9];
    M[2][2] = m[10];
    M[2][3] = m[11];
    M[3][0] = m[12];
    M[3][1] = m[13];
    M[3][2] = m[14];
    M[3][3] = 1.0f;

    return M;
}

void Scene::updateCameraPosition(double x, double y) {
    if(! control->dragged())
        return;

    control->rotate(mCamera.orientation, x, y);
    control->dragUpdate(x, y);
}

void Scene::updateCameraZoom(double x, double y) {
    
    if((3.0 + (mCamera.zoom - y / 5.0f)) > 0.1f)
        mCamera.zoom -= y / 5.0f;
}

void Scene::resetCamera() {
    glm::quat identityQuat;
    mCamera.orientation = identityQuat;
    mCamera.zoom = 2.0;
}

void Scene::stepSimulation() { 
    
    if(playPhysics) {
        btTransform worldTrans;
        btQuaternion rotation;
        btScalar rotAngle;
        btVector3 rotAxis;
       // float trans[16];
        Vector3<float> prevPos;
        float prevAngle;

        physicsWorld->stepSimulation(mSceneMatrices[I_MVP]);
     
        for(unsigned int i = 0; i < mGeometries.size(); i++) {
            if(mGeometries[i]->getType() == HALFEDGEMESH) {
                physicsWorld->getRigidBodyAt(i)->getMotionState()->getWorldTransform(worldTrans);
                float bulletTransform[16];
                worldTrans.getOpenGLMatrix(bulletTransform);

                mGeometries[i]->updateMesh(toGlmMat4(bulletTransform));
                updatePreComputedVoronoiPattern(mGeometries[i]->getObjName(), toGlmMat4(bulletTransform));
            }
        }
    }
}

void Scene::splitMesh(HalfEdgeMesh *he) {

    if(he->isCompoundComputed()) {

        ClippingMesh * cm = new ClippingMesh(he);
        
        int counter = 0;
        for(unsigned int i = 0; i < he->getNumVoronoiPoints(); i++) {
            counter++;
            addGeometry(cm->clipMesh(he->getVoronoiPoint(i)), DYNAMIC);
            mGeometries.back()->initialize(mPointLight.position);
        }
        
        delete mGeometries[1];
        mGeometries.erase(mGeometries.begin()+1);
        physicsWorld->removeGeometry(1);
        delete cm;
    }
}


void Scene::updatePreComputedVoronoiPattern(std::string s, glm::mat4 M) {

    Matrix4x4<float> transformMatrix = toMatrix4x4Row(M);
 
    for(std::vector<Vector3<float> >::iterator it = mVoronoiPoints[s].begin(); it != mVoronoiPoints[s].end(); ++it) {
        Vector4<float> p = transformMatrix * Vector4<float>((*it)[0], (*it)[1], (*it)[2], 1.0f);
        (*it) = Vector3<float>(p[0], p[1], p[2]);
    }
}
