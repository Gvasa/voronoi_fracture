#include "Scene.h"

struct cameraHandler {
    float fov = 45.0f;
    float aspectRatio = 4.0f / 3.0f;
    float zoom = 1.0f;
    glm::quat orientation;

    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
};

cameraHandler camera;

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

    camera.projectionMatrix = glm::perspective(
        camera.fov,          // field of view, 45.0
        camera.aspectRatio,  // 4/3 atm
        0.1f,                // Near clipping plane
        100.0f);             // far clipping plane

    camera.viewMatrix = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f+camera.zoom),            // Camera / eye position
        glm::vec3(0.0f, 0.0f, 0.0f),            // Target, what to look at
        glm::vec3(0.0f, 1.0f, 0.0f)) *          // Up-vector                            
        glm::mat4_cast(camera.orientation);     // multiplies the veiw matrix with current rotation

    glm::mat4 modelMatrix;
    // render Geometries in scene
    for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it) {

        // The scene modelmatrix is nothing atm, the geometries will have their own model transforms
        modelMatrix = (*it)->getDrawMat();

        // Construct MVP matrix
        mSceneMatrices[I_MVP] = toMatrix4x4(camera.projectionMatrix * camera.viewMatrix * modelMatrix);

        // Modelview Matrix, apply camera transforms here as well
        mSceneMatrices[I_MV] = toMatrix4x4(camera.viewMatrix * modelMatrix);

        // Modelview Matrix for our light
        mSceneMatrices[I_MV_LIGHT] = toMatrix4x4(camera.viewMatrix * modelMatrix);

        // Normal Matrix, used for normals in our shading
        mSceneMatrices[I_NM] = toMatrix4x4(glm::inverseTranspose(glm::mat4(camera.viewMatrix * modelMatrix)));

        (*it)->render(mSceneMatrices);
    }

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}


void Scene::addGeometry(Geometry *G, unsigned int type) {
    mGeometries.push_back(G);
    G->calculateCenterOfMass();
    physicsWorld->addGeometry(G->getUniqueVertexList(), G->getCenterOfMass() , type);

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
    glm::mat4 M = glm::mat4(1.0f);

        M[0][0] = m[0];
        M[0][1] = m[1];
        M[0][2] = m[2];

        M[1][0] = m[4];
        M[1][1] = m[5];
        M[1][2] = m[6];

        M[2][0] = m[8];
        M[2][1] = m[9];
        M[2][2] = m[10];

        M[0][3] = m[12];
        M[1][3] = m[13];
        M[2][3] = m[14];

/*
        M[0][0] = m[0];
        M[1][0] = m[1];
        M[2][0] = m[2];
        M[3][0] = m[3];
        M[0][1] = m[4];
        M[1][1] = m[5];
        M[2][1] = m[6];
        M[3][1] = m[7];
        M[0][2] = m[8];
        M[1][2] = m[9];
        M[2][2] = m[10];
        M[3][2] = m[11];
        M[0][3] = m[12];
        M[1][3] = m[13];
        M[2][3] = m[14];
        M[3][3] = m[15];
*/
    return M;
}

void Scene::updateCameraPosition(double x, double y) {
    if(! control->dragged())
        return;

    control->rotate(camera.orientation, x, y);
    control->dragUpdate(x, y);
}

void Scene::updateCameraZoom(double x, double y) {
    
    if((3.0 + (camera.zoom - y / 5.0f)) > 0.1f)
        camera.zoom -= y / 5.0f;
}

void Scene::resetCamera() {
    glm::quat identityQuat;
    camera.orientation = identityQuat;
    camera.zoom = 2.0;
}

/*void Scene::stepSimulation() { 
    
    btTransform worldTrans;
    btQuaternion rotation;
    btScalar rotAngle;
    btVector3 rotAxis;
    Vector3<float> prevPos;
    debug
    std::vector<unsigned int> splitIndex = physicsWorld->stepSimulation(mSceneMatrices[I_MVP]);
    debug

    for(unsigned int i = 0; i < splitIndex.size(); i++ )
        splitMesh(i);

    debug

    std::cout << "\nmGeometries.size(): " << mGeometries.size() << std::endl;
    for(unsigned int i = 0; i < mGeometries.size(); i++) {

        std::cout << "\nmGeometry: " << i << std::endl;
        if(mGeometries[i]->getType() == HALFEDGEMESH) {
            debug
            physicsWorld->getRigidBodyAt(i)->getMotionState()->getWorldTransform(worldTrans);
            HalfEdgeMesh* hem = dynamic_cast<HalfEdgeMesh*>(mGeometries[i]);
            debug
            prevPos = hem->getPrevPos();
            debug

            rotation = worldTrans.getRotation();
            rotAxis = rotation.getAxis();
            rotAngle = rotation.getAngle();

            glm::mat4 fTrans = glm::mat4(1.0f);

            glm::vec3 rTrans(prevPos[0], prevPos[1], prevPos[2]);
            glm::vec3 rRotAxis(rotAxis.getX(), rotAxis.getY(), rotAxis.getZ());
            glm::vec3 tTrans(worldTrans.getOrigin().getX() - prevPos[0], worldTrans.getOrigin().getY() - prevPos[1], worldTrans.getOrigin().getZ() - prevPos[2]);

            fTrans = glm::translate(fTrans, -rTrans);
            fTrans = glm::rotate(fTrans, rotAngle, rRotAxis);
            fTrans = glm::translate(fTrans, rTrans);

            std::cout << "-----------\n\nrotAngle: " << rotAngle << std::endl;
            std::cout << "axis: " << rotAxis.getX() << ", " << rotAxis.getY() << ", " << rotAxis.getZ() << std::endl;

            fTrans = glm::translate(fTrans, tTrans);
            debug
            float bulletTransform[16];
            worldTrans.getOpenGLMatrix(bulletTransform);
            debug
            mGeometries[i]->setDrawMat(fTrans);
            debug
            hem->setCalcMat(fTrans);
            debug
            hem->setPrevPos(Vector3<float>(worldTrans.getOrigin().getX(), worldTrans.getOrigin().getY(),worldTrans.getOrigin().getZ()));
            debug
        }
    }
    debug
}*/

void Scene::stepSimulation() { 
    btTransform worldTrans;
    btQuaternion rotation;
    btScalar rotAngle;
    btVector3 rotAxis;
   // float trans[16];
    Vector3<float> prevPos;
    float prevAngle;

    std::vector<unsigned int> splitIndex = physicsWorld->stepSimulation(mSceneMatrices[I_MVP]);

    for(unsigned int i = 0; i < splitIndex.size(); i++ )
        splitMesh(i);

    //physicsWorld->stepSimulation(mSceneMatrices[I_MVP]);

    for(unsigned int i = 0; i < mGeometries.size(); i++) {
        if(mGeometries[i]->getType() == HALFEDGEMESH) {
                physicsWorld->getRigidBodyAt(i)->getMotionState()->getWorldTransform(worldTrans);
                float bulletTransform[16];
                worldTrans.getOpenGLMatrix(bulletTransform);

                //mGeometries[i]->setTransMat(toGlmMat4(bulletTransform));
                mGeometries[i]->setCalcMat(toGlmMat4(bulletTransform));
                mGeometries[i]->setDrawMat(toGlmMat4(bulletTransform));
        }
    }
}

void Scene::splitMesh(unsigned int i) {
    if(mSplit && i == 1) {

        if(mGeometries[i]->getType() == HALFEDGEMESH) {

            dynamic_cast<HalfEdgeMesh*>(mGeometries[i])->computeVoronoiPattern();
            
            splitMesh(dynamic_cast<HalfEdgeMesh*>(mGeometries[i]));
            
            mSplit = false;
        }  
    }
}

void Scene::splitMesh(HalfEdgeMesh *he) {
    debug
    if(he->isCompoundComputed()) {

        std::cout << "\nNUMBER OF VORONOI POINTS: " << he->getNumVoronoiPoints() << std::endl;

        std::vector<Vector3<float> > VP = he->getVoronoiPoints();

        for(unsigned int i = 0; i < VP.size(); i++)
            std::cout << "\nVoronoi position: " << VP[i] << std::endl;

        ClippingMesh * cm = new ClippingMesh(he);
        debug
        int counter = 0;

        for(unsigned int i = 0; i < he->getNumVoronoiPoints(); i++) {
            counter++;
            debug
            addGeometry(cm->clipMesh(he->getVoronoiPoint(i)), DYNAMIC);
            debug
            mGeometries.back()->initialize(mPointLight.position);
            debug
        }

        debug
        delete mGeometries[1];
        mGeometries.erase(mGeometries.begin()+1);
        physicsWorld->removeGeometry(1);
        delete cm;

        std::cout << "\n\nGeometries left after splitting: \n";
        debug
        for(std::vector<Geometry*>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it)
            std::cout << "Type: " << (*it)->getType() << std::endl;
    }

    debug
}
