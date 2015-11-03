#include "Scene.h"

struct cameraHandler {
    float fov = 45.0f;
    float aspectRatio = 4.0f / 3.0f;
    glm::quat orientation;

    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
};

cameraHandler camera;

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

    // Init the lightsource parameters
    mPointLight.position = Vector3<float>(0.0f, 0.0f, 3.0f);
    mPointLight.color = Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f);

    // Background color
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

    mSceneMatrices.push_back(Matrix4x4<float>());
    mSceneMatrices.push_back(Matrix4x4<float>());
    mSceneMatrices.push_back(Matrix4x4<float>());
    mSceneMatrices.push_back(Matrix4x4<float>());

    for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it)
        (*it)->initialize(mPointLight.position);
}

// Draw all geometries
void Scene::draw() {

    glEnable( GL_CULL_FACE );
    //glEnable(GL_DEPTH_TEST);


    camera.projectionMatrix = glm::perspective(
        camera.fov,          // field of view, 45.0
        camera.aspectRatio,  // 4/3 atm
        0.1f,                // Near clipping plane
        100.0f);             // far clipping plane

    camera.viewMatrix = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f),            // Camera / eye position
        glm::vec3(0.0f, 0.0f, 0.0f),            // Target, what to look at
        glm::vec3(0.0f, 1.0f, 0.0f)) *          // Up-vector                            
        glm::mat4_cast(camera.orientation);     // multiplies the veiw matrix with current rotation

    // The scene modelmatrix is nothing atm, the geometries will have their own model transforms
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // Construct MVP matrix
    mSceneMatrices[I_MVP] = toMatrix4x4(camera.projectionMatrix * camera.viewMatrix * modelMatrix);

    // Modelview Matrix, apply camera transforms here as well
    mSceneMatrices[I_MV] = toMatrix4x4(camera.viewMatrix * modelMatrix);

    // Modelview Matrix for our light
    mSceneMatrices[I_MV_LIGHT] = toMatrix4x4(camera.viewMatrix * modelMatrix);

    // Normal Matrix, used for normals in our shading
    mSceneMatrices[I_NM] = toMatrix4x4(glm::inverseTranspose(glm::mat4(camera.viewMatrix * modelMatrix)));

    // Draw Geometries in scene
    for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it)
        (*it)->draw(mSceneMatrices);

    //glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
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


Matrix4x4<float> Scene::toMatrix4x4(glm::mat3 m) {
    float M[4][4] = {
        {m[0][0], m[0][1], m[0][2], 0.0f},
        {m[1][0], m[1][1], m[1][2], 0.0f},
        {m[2][0], m[2][1], m[2][2], 0.0f},
        {0.0f   , 0.0f   , 0.0f   , 1.0f}
    };
    return Matrix4x4<float>(M);
}


void Scene::updateCameraPosition(double x, double y) {
    if(! control.dragged())
        return;

    control.rotate(camera.orientation, x, y);
    control.dragUpdate(x, y);
}