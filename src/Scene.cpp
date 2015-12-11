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
        modelMatrix = (*it)->getTransMat();

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
    
    physicsWorld->addGeometry(G->getVertexList(), G->getCenterOfMass() , type);

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
        M[3][3] = m[15]; 

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

void Scene::stepSimulation() { 
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

                mGeometries[i]->setTransMat(toGlmMat4(bulletTransform));

                //btRigidBody* rigidBody = physicsWorld->getRigidBodyAt(i);
                //HalfEdgeMesh* hem = dynamic_cast<HalfEdgeMesh*>(mGeometries[i]);
                
//                 prevPos = hem->getPrevPos();
//                 prevAngle = hem->getPrevRot();

//                /* rotation = worldTrans.getRotation();
//                 rotAxis = rotation.getAxis();
//                 rotAngle = rotation.getAngle();
// */
//                 std::cout << "prevPos: " << prevPos << std::endl;

//                 btVector3 pos = rigidBody->getCenterOfMassPosition();
//                 rotation = rigidBody->getOrientation();
//                 rotAxis = rotation.getAxis();
//                 rotAngle = rotation.getAngle();
//                 //std::cout << "worldTransgetOrigin: " 
//                 //std::cout << "translate with: " << (float)worldTrans.getOrigin().getX() - prevPos[0] << " " <<  (float)worldTrans.getOrigin().getY() - prevPos[1] << " " << (float)worldTrans.getOrigin().getZ() - prevPos[2] << std::endl;
//                // std::cout << "translate with: " << (float)worldTrans.getOrigin().getX() - prevPos[0] << " " <<  (float)worldTrans.getOrigin().getY() - prevPos[1] << " " << (float)worldTrans.getOrigin().getZ() - prevPos[2] << std::endl;

//                 //if((float)rotAngle - prevAngle < -EPSILON || (float)rotAngle - prevAngle > EPSILON ) {
//                     //std::cout << " rotAxis: " << rotAxis.getX() << " " << rotAxis.getY() << " " << rotAxis.getZ() << "  - angle " << rotAngle << std::endl;
//                     hem->translate(-prevPos);
//                     hem->rotate(Vector3<float>((float)rotAxis.getX(), (float)rotAxis.getY(), (float)rotAxis.getZ() ), (float)rotAngle - prevAngle);
//                     hem->translate(prevPos);
//                     hem->setPrevRot((float)rotAngle);
//                 //}
                
//                 //hem->translate(Vector3<float>(((float)worldTrans.getOrigin().getX() - prevPos[0]),( (float)worldTrans.getOrigin().getY() - prevPos[1]), ((float)worldTrans.getOrigin().getZ() - prevPos[2])));
//                 //hem->setPrevPos(Vector3<float>(worldTrans.getOrigin().getX(), worldTrans.getOrigin().getY(), worldTrans.getOrigin().getZ()));
                
//                 hem->translate(Vector3<float>((float)pos.getX() - prevPos[0], (float)pos.getY() - prevPos[1], (float)pos.getZ() - prevPos[2]));
//                 hem->setPrevPos(Vector3<float>(pos.getX(), pos.getY(), pos.getZ()));
            

                /**********************************************
                /*
                /*  TYDLIGEN SÅ PÅVERKAS BULLET AV VAFAN MESHET HÅLLER PÅ MED! ; HEEEELT JÄVLA ORIMLIGT
                /*
                /*
                ********************************************* */

               //hem->setPrevRot((float)rotAngle);
        }
    }
    
}